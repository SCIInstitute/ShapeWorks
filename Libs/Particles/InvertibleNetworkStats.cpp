#pragma once
#include "InvertibleNetworkStats.h"
#include <torch/script.h>

namespace InvertibleNetStats
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
            if(j.contains("model_path")) this->m_model_path = j.at("model_path");
            if(j.contains("device_id")) this->m_device_id = j.at("device_id").get<nlohmann::json::number_integer_t>();
            if(j.contains("seed")) this->m_seed_val = j.at("seed").get<nlohmann::json::number_integer_t>();

        } catch (nlohmann::json::exception &e) {
            throw std::runtime_error("Unabled to parse json param file " + filename + " : " + e.what());
        }
        std::cout << "Inv Net Model Params Loaded from JSON file for Analysis " << this->m_model_path << " device " << this->m_device_id << std::endl;
        this->InitializeModel();
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
            const std::string fn = this->m_model_path;
            torch::jit::script::Module m = torch::jit::load(fn, this->m_device);
            this->m_module = m;
            this->m_module_exists = true;
            torch::manual_seed(this->m_seed_val);
            torch::cuda::manual_seed(this->m_seed_val);
            std::cout << " Torchscript Module loaded succesfully." << std::endl;
        }
        catch (const c10::Error& e) {
            std::cerr << "Error loading the pre-trained model in Analyze | " << e.what(); 
            return -1;
        }
        return 0;
    }

    //-----------------------------------------------------------------------------
    torch::Tensor Model::ForwardPass(torch::Tensor& input_tensor)
    {
        try
        {
            torch::NoGradGuard no_grad;
            this->m_module.eval();
            std::vector<torch::jit::IValue> inputs;
            inputs.push_back(input_tensor.to(this->m_device));
            auto outputs = this->m_module.forward(inputs).toTuple();
            torch::Tensor z0_particles = outputs->elements()[0].toTensor();
            z0_particles = z0_particles.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
            return z0_particles;
        }
        catch (const c10::Error& e) {
            std::cerr << "Error in Forward Pass | " << e.what(); 
        }
    }

    //-----------------------------------------------------------------------------
    torch::Tensor Model::InversePass(torch::Tensor& input_tensor)
    {
        try
        {
            torch::NoGradGuard no_grad;
            this->m_module.eval();
            std::vector<torch::jit::IValue> inputs;
            inputs.push_back(input_tensor.to(this->m_device));
            auto outputs = this->m_module.get_method("inverse")(inputs).toTuple();
            torch::Tensor z0_particles = outputs->elements()[0].toTensor();
            z0_particles = z0_particles.to(torch::TensorOptions(torch::kCPU).dtype(at::kDouble)); 
            return z0_particles;
        }
        catch (const c10::Error& e) {
            std::cerr << "Error in Inverse Pass | " << e.what(); 
        }
    }

    std::vector<Eigen::VectorXd> Model::ForwardPass(std::vector<Eigen::VectorXd> points)
    {
        unsigned int num_shapes = points.size(); //dps = 1
        unsigned int dM = points[0].size();
        Eigen::MatrixXd shape_matrix; 
        shape_matrix.resize(num_shapes, dM);
        for (size_t i = 0; i < num_shapes; ++i)
        {
            shape_matrix.row(i) = points[i];
        }
        torch::Tensor z_particles = torch::from_blob(shape_matrix.data(), {num_shapes, dM});
        torch::Tensor z0_particles = this->ForwardPass(z_particles);
        std::vector<Eigen::VectorXd> z0_points;
        for (size_t i = 0; i < num_shapes; ++i)
        {
            auto T = z0_particles[i];
            double* data = T.data_ptr<double>();
            Eigen::Map<Eigen::VectorXd> v(data, dM);
            z0_points.push_back(v);
        }
        return z0_points;
    }

    Eigen::VectorXd Model::InversePass(Eigen::VectorXd point)
    {
        unsigned int dM = point.size();
        torch::Tensor z0_particles = torch::from_blob(point.data(), {dM});
        torch::Tensor z_particles = this->InversePass(z0_particles);
        double* data = z_particles.data_ptr<double>();
        Eigen::Map<Eigen::VectorXd> v(data, dM);
        return v;
    }
}