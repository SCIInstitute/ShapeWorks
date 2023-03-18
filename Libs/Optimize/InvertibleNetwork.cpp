#pragma once
#include "InvertibleNetwork.h"
#include <torch/script.h>


namespace InvertibleNet
{
    //-----------------------------------------------------------------------------
    void Model::LoadParams(const std::string filename) {
        std::ifstream in(filename);
        if (!in.good()) {
            throw std::runtime_error("Unable to open InvNet Model params " + filename);
        }
        try {
            nlohmann::json j;
            in >> j;
            if(j.contains("learning_rate")) this->m_learning_rate = j.at("learning_rate").get<double>();
            if(j.contains("model_path")) this->m_model_path = j.at("model_path");
            if(j.contains("save_model_path")) this->m_save_model_path = j.at("save_model_path");
            if(j.contains("n_epochs")) this->m_n_epochs = j.at("n_epochs").get<nlohmann::json::number_integer_t>();
            if(j.contains("batch_size")) this->m_batch_size = j.at("batch_size").get<nlohmann::json::number_integer_t>();
            if(j.contains("device_id")) this->m_device_id = j.at("device_id").get<nlohmann::json::number_integer_t>();
            if(j.contains("latent_dimension")) this->m_latent_dimension = j.at("latent_dimension").get<nlohmann::json::number_integer_t>();
            if(j.contains("seed")) this->m_seed_val = j.at("seed").get<nlohmann::json::number_integer_t>();
            if(j.contains("optimizer"))
            {
                std::string opt_type = j.at("optimizer");
                if (opt_type == "sgd")
                {
                    this->m_optimizer = InvertibleNet::OptimizerType::SGD;
                }
                else if (opt_type == "adam")
                {
                    this->m_optimizer = InvertibleNet::OptimizerType::Adam;
                }
            }


        } catch (nlohmann::json::exception &e) {
            throw std::runtime_error("Unabled to parse json param file " + filename + " : " + e.what());
             std::exit(EXIT_FAILURE);
        }
        std::cout << "Inv Net Model Params Loaded from JSON file " << this->m_model_path << " device " << this->m_device_id << "latent dims = " << this->m_latent_dimension << std::endl;
        this->InitializeModel();
    }

    //-----------------------------------------------------------------------------
    int Model::InitializeModel()
    {
        try {
            if (this->m_device_id >= 0)
            {
                this->m_device = torch::Device(torch::kCUDA, m_device_id);
            }
            else{
                this->m_device = torch::Device(torch::kCPU);
            }
            const std::string fn = this->m_model_path;
            torch::jit::script::Module m = torch::jit::load(fn, this->m_device);
            this->m_module = m;
            // this->m_module.to(this->m_device);
            this->m_module_exists = true;
            torch::manual_seed(this->m_seed_val);
            torch::cuda::manual_seed(this->m_seed_val);
            std::cout << " Torchscript Module loaded succesfully." << std::endl;
        }
        catch (const c10::Error& e) {
            std::cerr << "Error loading the Trained Network in SW | " << e.what(); 
            std::exit(EXIT_FAILURE);
            return -1;
        }
        return 0;
    }


    //-----------------------------------------------------------------------------
    void Model::ComputeEnergy(Tensor& input_tensor, double& energy_in_data_space)
    {
        try
        {
            torch::NoGradGuard no_grad;
            this->m_module.eval();
            input_tensor.to(torch::kFloat);
            std::vector<torch::jit::IValue> inputs;
            inputs.push_back(input_tensor.to(this->m_device)); // 1 X dM
            auto outputs = this->m_module.get_method("forward_pass_complete")(inputs).toTuple();
            auto out_tensor = outputs->elements()[4].toTensor().to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble));
            energy_in_data_space = out_tensor.item<double>();
            MSG("Forward Pass for Computing Energy done !!! ");
        }
        catch (const c10::Error& e) {
            std::cerr << "Error in Libtorch Operations for Compute Energy | " << e.what(); 
            std::exit(EXIT_FAILURE);
        }
    }

    //-----------------------------------------------------------------------------
    void Model::ForwardPass(Tensor& input_tensor, Tensor& output_tensor, Tensor& jacobian_matrix, double& log_prob_u, double& log_det_g, double& log_det_j)
    {
        try
        {
            this->m_module.eval();
            std::vector<torch::jit::IValue> inputs; // 1 X dM
            input_tensor.requires_grad_(true);
            input_tensor.to(torch::kFloat);
            inputs.push_back(input_tensor.to(this->m_device));

            auto outputs = this->m_module.get_method("forward_pass_complete")(inputs).toTuple();
            MSG("Forward Pass for Grad Updates Done!!!");
            output_tensor = outputs->elements()[0].toTensor(); // 1 X L // GPU

            auto log_prob_u_ten = outputs->elements()[1].toTensor();
            log_prob_u_ten = log_prob_u_ten.to(torch::TensorOptions(torch::kCPU).dtype(torch::kDouble)); 
            log_prob_u = log_prob_u_ten.item<double>();

            auto log_det_g_ten = outputs->elements()[2].toTensor();
            log_det_g_ten = log_det_g_ten.to(torch::TensorOptions(torch::kCPU).dtype(torch::kDouble)); 
            log_det_g = log_det_g_ten.item<double>();

            auto log_det_j_ten = outputs->elements()[3].toTensor();
            log_det_j_ten = log_det_j_ten.to(torch::TensorOptions(torch::kCPU).dtype(torch::kDouble)); 
            log_det_j = log_det_j_ten.item<double>();

            this->ComputeJacobian(input_tensor, output_tensor, jacobian_matrix);
            output_tensor.to(torch::TensorOptions(torch::kCPU).dtype(torch::kDouble));
            jacobian_matrix.to(torch::TensorOptions(torch::kCPU).dtype(torch::kDouble));

            MSG("Log Values | "); DEBUG(log_prob_u); DEBUG(log_det_g); DEBUG(log_det_j); 
        }
        catch (const c10::Error& e) {
            std::cout << "Error in Forward Pass during Energy/Gradient Computations | " << e.what();
             std::exit(EXIT_FAILURE);
        }
    }

    void Model::ComputeJacobian(Tensor& input, Tensor& output, Tensor& jacobian_matrix)
    {
        // inputs: 1 X dM
        // outputs: 1 X L
        // jacobian : L X dM
        try{
            auto latent_dims = output.sizes()[1];
            auto data_dims = input.sizes()[1];
            input = input.view({data_dims}); output = output.view({latent_dims});

            auto grad_output = torch::ones({1}).to(this->m_device);
            
            std::vector<torch::Tensor> outs;
            std::vector<torch::Tensor> inputs;
            std::vector<torch::Tensor> grad_outputs;

            for (auto l = 0; l < latent_dims; ++l)
            {
                outs.push_back(output.index({torch::indexing::Slice(l, l+1)}));
                inputs.push_back(input);
                grad_outputs.push_back(grad_output);
            }

            jacobian_matrix = torch::zeros({latent_dims, data_dims}).to(this->m_device);
            
            for (auto l = 0; l < latent_dims; ++l)
            {
                auto gradient = torch::autograd::grad(/*outputs*/ {outs[l]}, /*inputs*/ {inputs[l]},
                                                    /*grad_outputs*/ {grad_outputs[l]}, /*retain_graph*/ true,
                                                    /*create_graph*/ true, /*allow_unused*/ false);

                jacobian_matrix.index_put_({torch::indexing::Slice(l, torch::indexing::None)}, gradient[0]);
            }
        }  
        catch (const c10::Error& e) {
            std::cout << "Error in Jacobian Computations | " << e.what();
            std::exit(EXIT_FAILURE);
        }

    }
        
}
