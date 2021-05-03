#include <pc_aux.h>

namespace shapeworks {

Eigen::MatrixXd pc_aux::compute_normals(const Eigen::MatrixXd & pts, double r){

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);

    cloud->points.resize(pts.rows() * 3);

    for(size_t i = 0; i < pts.rows(); i++){
        cloud->points[i].x = float(pts(i,0));
        cloud->points[i].y = float(pts(i,1));
        cloud->points[i].z = float(pts(i,2));
    }

    // Create the normal estimation class, and pass the input dataset to it
    pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> ne;
    ne.setInputCloud (cloud);

    // Create a KD-Tree
    pcl::search::KdTree<pcl::PointXYZ>::Ptr tree (new pcl::search::KdTree<pcl::PointXYZ>);

    // Output datasets
    pcl::PointCloud<pcl::Normal>::Ptr cloud_normals (new pcl::PointCloud<pcl::Normal>);

    // Use all neighbors in a sphere of radius 3cm
    ne.setRadiusSearch (r);
    ne.setSearchMethod(tree);

    // Compute the features
    ne.compute (*cloud_normals);

    /*
    // Output has the PointNormal type in order to store the normals calculated by MLS
    pcl::PointCloud<pcl::PointNormal> mls_points;

    // Init object (second point type is for the normals, even if unused)
    pcl::MovingLeastSquares<pcl::PointXYZ, pcl::PointNormal> mls;

    mls.setComputeNormals (true);

    // Set parameters
    mls.setInputCloud (cloud);
    mls.setPolynomialOrder (2);
    mls.setSearchMethod (tree);
    mls.setSearchRadius (0.03);

    // Reconstruct
    mls.process (mls_points);
    */

    Eigen::MatrixXd norms = Eigen::MatrixXd(pts.rows(), 3);

    for(size_t i = 0; i < pts.rows(); i++){
        norms(i,0) = double(cloud_normals->points[i]._Normal::normal_x);
        norms(i,1) = double(cloud_normals->points[i]._Normal::normal_y);
        norms(i,2) = double(cloud_normals->points[i]._Normal::normal_z);
    }

    std::cout << norms << std::endl;
    /*
    // visualize normals
    pcl::visualization::PCLVisualizer viewer("PCL Viewer");
    viewer.setBackgroundColor (0.0, 0.0, 0.5);
    viewer.addPointCloudNormals<pcl::PointXYZ,pcl::Normal>(cloud, &mls);

    while (!viewer.wasStopped ())
    {
        viewer.spinOnce ();
    }
    */

    return norms;
}

}
