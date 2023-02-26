#pragma once
#include <torch/script.h>
#include <torch/torch.h>
#include <nlohmann/json.hpp>

namespace InvertibleNetStats
{

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
            m_model_path = "";
            m_device_id = -1;
            m_seed_val = 1;
        }
        virtual ~Model(){};

        torch::Device GetDevice()
        {
            return this->m_device;
        };

        int InitializeModel();
        bool GetModelInitialized()
        {
            return this->m_module_exists;
        };

        void LoadParams(const std::string filepath);

        torch::Tensor ForwardPass(torch::Tensor& input_tensor);

    private:
        torch::jit::script::Module m_module;
        bool m_module_exists;
        torch::Device m_device = torch::kCPU;

        // hyper-params
        std::string m_model_path;
        int m_seed_val;
        int m_device_id;
};

}