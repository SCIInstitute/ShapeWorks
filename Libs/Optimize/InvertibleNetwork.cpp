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
        std::cout << "Inv Net Model Params Loaded from JSON file " << this->m_model_path << " device " << this->m_device_id << std::endl;
        this->InitializeModel();
    }

    //-----------------------------------------------------------------------------
    int Model::TrainModel(torch::Tensor& shape_matrix)
    {
        try
        {
            this->m_module.train();
            std::vector<at::Tensor> parameters;
            for (const auto& params : this->m_module.parameters()) {
                parameters.push_back(params);
            }
            torch::optim::SGD optimizer (parameters, /*lr=*/0.1);
            if (this->m_optimizer == InvertibleNet::OptimizerType::SGD)
            {
                torch::optim::SGD optimizer(parameters, /*lr=*/this->m_learning_rate);
            }
            else if (this->m_optimizer == InvertibleNet::OptimizerType::Adam)
            {
                torch::optim::Adam optimizer(parameters, torch::optim::AdamOptions(this->m_learning_rate /*learning rate*/));
            }

            std::cout << "Optim::Optimizer  " << parameters.size() <<  std::endl;

            auto custom_dataset = InvertibleNet::CustomDataset(shape_matrix).map(torch::data::transforms::Stack<>());
            auto data_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(std::move(custom_dataset), this->m_batch_size);
            std::cout << "Dataloader initalized" << std::endl;
            
            std::vector<torch::jit::IValue> new_mean;
            new_mean.push_back(this->m_base_dist_mean.to(this->m_device));
            auto mean_ = this->m_module.get_method("set_base_dist_mean")(new_mean).toTensor();

            std::vector<torch::jit::IValue> new_cov;
            new_cov.push_back(this->m_base_dist_mean.to(this->m_device));
            auto cov_ = this->m_module.get_method("set_base_dist_var")(new_cov).toTensor();

            std::cout << "********* Training the Invertible Network **********" <<  std::endl;
            float batch_index = 0;
            for(unsigned int i = 0; i < this->m_n_epochs; ++i) {
                for(auto& batch: *data_loader) {
                    auto data = batch.data;
                    // Should be of length: batch_size
                    data = data.to(torch::kF32);
                    std::vector<torch::jit::IValue> input;
                    input.push_back(data.to(this->m_device));
                    optimizer.zero_grad();
                    // auto output = this->m_module.forward(input).toTuple();
                    auto outputs = this->m_module.get_method("log_prob")(input).toTuple();
                    auto loss = -((outputs->elements()[0].toTensor()).mean(0));
                    loss.backward();
                    optimizer.step();                
                    batch_index += 1;
                    if (i % this->m_log_interval == 0)
                    {
                        std::cout << "Epoch: " << i  << ", " << "Loss: " << loss.item<double>() << std::endl;
                    }
                }
                this->m_module.save(this->m_save_model_path + "/model_libtorch.pt");
            }
        }  
        catch (const c10::Error& e) {
            std::cerr << "Error while training the model in SW | " << e.what(); 
            std::exit(EXIT_FAILURE);
            return -1;
        }
       
    }

    //-----------------------------------------------------------------------------
    int Model::InitializeModel()
    {
        try {
            if (this->m_device_id >= 0)
            {
                this->m_device = torch::Device(torch::kCUDA, 0);
            }
            else{
                this->m_device = torch::Device(torch::kCPU);
            }
            // std::cout << " Initializing 1 " << std::endl;
            const std::string fn = this->m_model_path;
            torch::jit::script::Module m = torch::jit::load(fn, this->m_device);
            // std::cout << " Initializing 2 " << std::endl;
            this->m_module = m;
            // std::cout << " Initializing 3 " << std::endl;
            // this->m_module.to(this->m_device);
            this->m_module_exists = true;
            torch::manual_seed(this->m_seed_val);
            torch::cuda::manual_seed(this->m_seed_val);
            std::cout << " Torchscript Module loaded succesfully." << std::endl;
        }
        catch (const c10::Error& e) {
            std::cerr << "Error loading the pre-trained model in SW | " << e.what(); 
            std::exit(EXIT_FAILURE);
            return -1;
        }
        return 0;
    }

    //-----------------------------------------------------------------------------
    void Model::ForwardPass(torch::Tensor& input_tensor, torch::Tensor& out_tensor)
    {
        // Forward Pass for latent space initialization
        try
        {
            torch::NoGradGuard no_grad;
            this->m_module.eval();
            std::vector<torch::jit::IValue> inputs;
            inputs.push_back(input_tensor.to(this->m_device)); // 1 X dM
            auto outputs = this->m_module.get_method("forward_for_cpp")(inputs).toTuple();
            out_tensor = outputs->elements()[0].toTensor().to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); // 1 X L
            MSG("Forward Pass done");
        }
        catch (const c10::Error& e) {
            std::cerr << "Error in General Forward Pass | " << e.what(); 
            std::exit(EXIT_FAILURE);
        }
    }

    //-----------------------------------------------------------------------------
    void Model::RunForwardPassWithJacobian(double& log_prob_u, double& log_det_g, double& log_det_j, torch::Tensor& input_tensor, torch::Tensor& jacobian_matrix) // for gradient update
    {
        // Forward Pass for gradient updates
        try
        {
            this->m_module.eval();
            std::vector<torch::jit::IValue> inputs; // 1 X dM
            inputs.push_back(input_tensor.to(this->m_device));
            auto outputs = this->m_module.get_method("forward_for_cpp")(inputs).toTuple();
            MSG("Forward Pass Done!!!");
            auto log_prob_u_ten = outputs->elements()[1].toTensor();
            log_prob_u_ten = log_prob_u_ten.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
            log_prob_u = log_prob_u_ten.item<double>();

            auto log_det_g_ten = outputs->elements()[2].toTensor();
            log_det_g_ten = log_det_g_ten.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
            log_det_g = log_det_g_ten.item<double>();

            auto log_det_j_ten = outputs->elements()[3].toTensor();
            log_det_j_ten = log_det_j_ten.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
            log_det_j = log_det_j_ten.item<double>();

            jacobian_matrix = outputs->elements()[3].toTensor().to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble));

            MSG("Log Values | "); DEBUG(log_prob_u); DEBUG(log_det_g); DEBUG(log_det_j); 
        }
        catch (const c10::Error& e) {
            std::cout << "Error in Forward Pass during Energy/Gradient Computations | " << e.what();
             std::exit(EXIT_FAILURE);
        }
    }

}
