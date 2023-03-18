#pragma once
#include <torch/script.h>
#include <torch/torch.h>
#include <nlohmann/json.hpp>
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include <assert.h>

#define MSG(x) do { std::cerr << x << std::endl; } while (0)
#define DEBUG(x) do { std::cerr << #x << ": " << x << std::endl; } while (0)

namespace InvertibleNet
{
    using Tensor = torch::Tensor;

enum class OptimizerType : int {
    SGD = 0,
    Adam = 1
};

class CustomDataset : public torch::data::Dataset<CustomDataset> {
private:
    torch::Tensor shape_matrix;
    size_t ds_size;
public:
    CustomDataset(torch::Tensor input_shape_matrix) {
        shape_matrix = input_shape_matrix;
        ds_size = input_shape_matrix.sizes()[0];
    };
    
    torch::data::Example<> get(size_t ind) override {
        int ind_ = static_cast<int>(ind);
        torch::Tensor x = shape_matrix.index({ind_, torch::indexing::Ellipsis});
        torch::Tensor y = shape_matrix.index({ind_, torch::indexing::Ellipsis});
        return {x.clone(), y.clone()};
    };
    
    torch::optional<size_t> size() const override {
        return ds_size;
    };
};

class Model
{
    public:
        using Pointer = std::shared_ptr<Model>;
        Model()
        {
            m_module_exists = false;
            m_n_epochs = 10;
            m_save_model_path = "";
            m_model_path = "";
            m_learning_rate = 1e-4;
            m_device_id = -1;
            m_optimizer = InvertibleNet::OptimizerType::SGD;
            m_batch_size = 4;
            m_log_interval = 10;
            m_seed_val = 1;
        }
        virtual ~Model(){};

        torch::Device GetDevice()
        {
            return this->m_device;
        };

        int GetLatentDimensions()
        {
            return this->m_latent_dimension;
        };


        int InitializeModel();
        bool GetModelInitialized()
        {
            return this->m_module_exists;
        };

        void LoadParams(const std::string filepath);
        void ForwardPass(Tensor& input_tensor, Tensor& output_tensor, Tensor& jacobian_matrix, double& log_prob_u, double& log_det_g, double& log_det_j);
        void ComputeEnergy(Tensor& input_tensor, double& energy_in_data_space);
        void ComputeJacobian(Tensor& inputs, Tensor& output, Tensor& jacobian_matrix);
    private:
        torch::jit::script::Module m_module;
        bool m_module_exists;
        torch::Device m_device = torch::kCPU;

        // hyper-params
        std::string m_save_model_path;
        std::string m_model_path;
        double m_learning_rate;
        int m_seed_val;
        int m_n_epochs;
        int m_device_id;
        int m_batch_size;

        int m_latent_dimension;
        unsigned int m_log_interval;
        InvertibleNet::OptimizerType m_optimizer;
};

}