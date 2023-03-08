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
    torch::Tensor Model::ForwardPass(torch::Tensor& input_tensor)
    {
        try
        {
            // std::cout << "FwdPass 1 " << std::endl;
            torch::NoGradGuard no_grad;
            this->m_module.eval();
            std::vector<torch::jit::IValue> inputs;
            inputs.push_back(input_tensor.to(this->m_device));
            auto outputs = this->m_module.forward(inputs).toTuple();
            // std::cout << "FwdPass 2 " << std::endl;
            torch::Tensor z0_particles = outputs->elements()[0].toTensor();
            // std::cout << "FwdPass 3 " << std::endl;
            z0_particles = z0_particles.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
            // std::cout << "FwdPass 4 " << std::endl;
            return z0_particles;
        }
        catch (const c10::Error& e) {
            std::cerr << "Error in General Forward Pass | " << e.what(); 
            std::exit(EXIT_FAILURE);
        }
    }

    //-----------------------------------------------------------------------------
    void Model::ForwardPass(torch::Tensor& input_tensor, torch::Tensor& jacobian_matrix, double& log_det_jacobian_val, torch::Tensor& p_z_0)
    {
        try
        {
            this->m_module.eval();
            // std::cout << "Forward Pass 1 " << std::endl;
            unsigned int dM = input_tensor.sizes()[0];
            torch::Tensor input_t = input_tensor.repeat({dM, 1}); // for jacobian
            // std::cout << "Forward Pass 1 " << std::endl;
            input_t.set_requires_grad(true);
            // std::cout << "Forward Pass 2 " << std::endl;
            std::vector<torch::jit::IValue> inputs;
            inputs.push_back(input_t.to(this->m_device));
            auto outputs = this->m_module.get_method("log_prob")(inputs).toTuple();
            // std::cout << "Forward Pass 3 " << std::endl;

            torch::Tensor z0_particles_tensor = outputs->elements()[0].toTensor();
            torch::Tensor ones = torch::ones({dM, dM}, this->m_device);
            // std::cout << "Forward Pass 4 " << std::endl;
            z0_particles_tensor.backward(ones);
            // std::cout << "Forward Pass 5 " << std::endl;
            jacobian_matrix = input_t.grad();
            // std::cout << "Forward Pass 6 " << std::endl;

            torch::Tensor log_det_jacobian_tensor = outputs->elements()[1].toTensor();
            // std::cout << "Forward Pass 7 " << std::endl;
            log_det_jacobian_tensor = log_det_jacobian_tensor.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
            std::cout << "Forward Pass 8 log_det_jacobian_tensor size =  " << log_det_jacobian_tensor.sizes() <<  std::endl;
            log_det_jacobian_val = log_det_jacobian_tensor[0].item<double>();

            std::vector<torch::jit::IValue> inputs_p;
            inputs_p.push_back(z0_particles_tensor[0].to(this->m_device));
            auto all_probs = this->m_module.get_method("base_dist_log_prob")(inputs_p).toTensor();
            p_z_0 = all_probs.view({1, dM});
            p_z_0 = p_z_0.exp();
            auto prob_is_nan = at::isnan(p_z_0).any().item<bool>();
            std::cout << "Forward Pass and Nan Present " << prob_is_nan << std::endl;

        }
        catch (const c10::Error& e) {
            std::cout << "Error in Forward Pass during Energy/Gradient Computations | " << e.what();
             std::exit(EXIT_FAILURE);
        }
    }

}
