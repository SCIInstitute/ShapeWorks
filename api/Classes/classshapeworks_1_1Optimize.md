---
title: shapeworks::Optimize

---

# shapeworks::Optimize



 [More...](#detailed-description)


`#include <Optimize.h>`

Inherited by [shapeworks::QOptimize](../Classes/classshapeworks_1_1QOptimize.md)

## Public Types

|                | Name           |
| -------------- | -------------- |
| using itk::Image< float, 3 > | **[ImageType](../Classes/classshapeworks_1_1Optimize.md#using-imagetype)**  |
| using [itk::ParticleVectorFunction](../Classes/classitk_1_1ParticleVectorFunction.md)< 3 >::VectorType | **[VectorType](../Classes/classshapeworks_1_1Optimize.md#using-vectortype)**  |
| using Eigen::MatrixXd | **[MatrixType](../Classes/classshapeworks_1_1Optimize.md#using-matrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Optimize](../Classes/classshapeworks_1_1Optimize.md#function-optimize)**()<br>Constructor.  |
| virtual | **[~Optimize](../Classes/classshapeworks_1_1Optimize.md#function-~optimize)**()<br>Destructor.  |
| bool | **[Run](../Classes/classshapeworks_1_1Optimize.md#function-run)**()<br>Run the optimization.  |
| bool | **[LoadParameterFile](../Classes/classshapeworks_1_1Optimize.md#function-loadparameterfile)**(std::string filename)<br>Load a parameter file.  |
| void | **[SetIterationCallbackFunction](../Classes/classshapeworks_1_1Optimize.md#function-setiterationcallbackfunction)**(const std::function< void(void)> & f) |
| void | **[AbortOptimization](../Classes/classshapeworks_1_1Optimize.md#function-abortoptimization)**()<br>Abort optimization.  |
| bool | **[GetAborted](../Classes/classshapeworks_1_1Optimize.md#function-getaborted)**()<br>Return if the optimization was aborted.  |
| virtual std::vector< std::vector< itk::Point< double > > > | **[GetLocalPoints](../Classes/classshapeworks_1_1Optimize.md#function-getlocalpoints)**()<br>Return the local points.  |
| virtual std::vector< std::vector< itk::Point< double > > > | **[GetGlobalPoints](../Classes/classshapeworks_1_1Optimize.md#function-getglobalpoints)**()<br>Return the global points.  |
| void | **[SetCutPlanes](../Classes/classshapeworks_1_1Optimize.md#function-setcutplanes)**(std::vector< std::array< itk::Point< double >, 3 >> cut_planes)<br>Set cutting planes.  |
| void | **[SetVerbosity](../Classes/classshapeworks_1_1Optimize.md#function-setverbosity)**(int verbosity_level)<br>Set the verbosity level (0-5)  |
| void | **[SetDomainsPerShape](../Classes/classshapeworks_1_1Optimize.md#function-setdomainspershape)**(int domains_per_shape)<br>Set the number of domains per shape.  |
| int | **[GetDomainsPerShape](../Classes/classshapeworks_1_1Optimize.md#function-getdomainspershape)**()<br>Return the number of domains per shape.  |
| void | **[SetDomainType](../Classes/classshapeworks_1_1Optimize.md#function-setdomaintype)**(shapeworks::DomainType type) |
| shapeworks::DomainType | **[GetDomainType](../Classes/classshapeworks_1_1Optimize.md#function-getdomaintype)**() |
| void | **[SetNumberOfParticles](../Classes/classshapeworks_1_1Optimize.md#function-setnumberofparticles)**(std::vector< int > number_of_particles)<br>Set the numbers of particles (vector of numbers, one for each domain)  |
| std::vector< int > | **[GetNumberOfParticles](../Classes/classshapeworks_1_1Optimize.md#function-getnumberofparticles)**()<br>Return the numbers of particles per domain.  |
| void | **[SetTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-settransformfile)**(std::string filename)<br>Set the transform file.  |
| std::string | **[GetTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-gettransformfile)**()<br>Get the transform file.  |
| void | **[SetPrefixTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-setprefixtransformfile)**(std::string prefix_transform_file)<br>Set the prefix transform file (TODO: details)  |
| std::string | **[GetPrefixTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-getprefixtransformfile)**()<br>Get the prefix transform file.  |
| void | **[SetOutputDir](../Classes/classshapeworks_1_1Optimize.md#function-setoutputdir)**(std::string output_dir)<br>Set the output directory.  |
| void | **[SetOutputTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-setoutputtransformfile)**(std::string output_transform_file)<br>Set the output transform file.  |
| void | **[SetOutputIndividualTransformFiles](../Classes/classshapeworks_1_1Optimize.md#function-setoutputindividualtransformfiles)**(bool value)<br>Set whether individual transforms should be written.  |
| void | **[SetUseMeshBasedAttributes](../Classes/classshapeworks_1_1Optimize.md#function-setusemeshbasedattributes)**(bool use_mesh_based_attributes)<br>Set if mesh based attributes should be used.  |
| bool | **[GetUseMeshBasedAttributes](../Classes/classshapeworks_1_1Optimize.md#function-getusemeshbasedattributes)**()<br>Get if mesh based attributes are being used.  |
| void | **[SetUseXYZ](../Classes/classshapeworks_1_1Optimize.md#function-setusexyz)**(std::vector< bool > use_xyz)<br>Set if XYZ is used, one value per domain.  |
| void | **[SetUseNormals](../Classes/classshapeworks_1_1Optimize.md#function-setusenormals)**(std::vector< bool > use_normals)<br>Set if Normals are being used, one value per domain.  |
| void | **[SetAttributesPerDomain](../Classes/classshapeworks_1_1Optimize.md#function-setattributesperdomain)**(std::vector< int > attributes_per_domain)<br>Set the attributes per domain.  |
| std::vector< int > | **[GetAttributesPerDomain](../Classes/classshapeworks_1_1Optimize.md#function-getattributesperdomain)**()<br>Get attributes per domain.  |
| void | **[SetDistributionDomainID](../Classes/classshapeworks_1_1Optimize.md#function-setdistributiondomainid)**(int distribution_domain_id)<br>Set the distribution domain ID (TODO: details)  |
| int | **[GetDistributionDomainID](../Classes/classshapeworks_1_1Optimize.md#function-getdistributiondomainid)**()<br>Get the distribution domain ID.  |
| void | **[SetOutputCuttingPlaneFile](../Classes/classshapeworks_1_1Optimize.md#function-setoutputcuttingplanefile)**(std::string output_cutting_plane_file)<br>Set the output cutting plane file.  |
| void | **[SetUseCuttingPlanes](../Classes/classshapeworks_1_1Optimize.md#function-setusecuttingplanes)**(bool use_cutting_planes)<br>Set if using cutting planes.  |
| void | **[SetCuttingPlane](../Classes/classshapeworks_1_1Optimize.md#function-setcuttingplane)**(unsigned int i, const vnl_vector_fixed< double, 3 > & va, const vnl_vector_fixed< double, 3 > & vb, const vnl_vector_fixed< double, 3 > & vc)<br>Set a given cutting plane for a shape.  |
| void | **[SetProcessingMode](../Classes/classshapeworks_1_1Optimize.md#function-setprocessingmode)**(int mode)<br>Set processing mode (TODO: details)  |
| void | **[SetAdaptivityMode](../Classes/classshapeworks_1_1Optimize.md#function-setadaptivitymode)**(int adaptivity_mode)<br>Set adaptivity mode (TODO: details)  |
| void | **[SetAdaptivityStrength](../Classes/classshapeworks_1_1Optimize.md#function-setadaptivitystrength)**(double adaptivity_strength)<br>Set adaptivity strength (TODO: details)  |
| void | **[SetPairwisePotentialType](../Classes/classshapeworks_1_1Optimize.md#function-setpairwisepotentialtype)**(int pairwise_potential_type)<br>Set pairwise potential type (TODO: details)  |
| void | **[SetTimePtsPerSubject](../Classes/classshapeworks_1_1Optimize.md#function-settimeptspersubject)**(int time_pts_per_subject)<br>Set the number of time points per subject (TODO: details)  |
| int | **[GetTimePtsPerSubject](../Classes/classshapeworks_1_1Optimize.md#function-gettimeptspersubject)**()<br>Get the number of time points per subject (TODO: details)  |
| void | **[SetOptimizationIterations](../Classes/classshapeworks_1_1Optimize.md#function-setoptimizationiterations)**(int optimization_iterations)<br>Set the number of optimization iterations.  |
| void | **[SetOptimizationIterationsCompleted](../Classes/classshapeworks_1_1Optimize.md#function-setoptimizationiterationscompleted)**(int optimization_iterations_completed)<br>Set the number of optimization iterations already completed (TODO: details)  |
| void | **[SetIterationsPerSplit](../Classes/classshapeworks_1_1Optimize.md#function-setiterationspersplit)**(int iterations_per_split)<br>Set the number of iterations per split.  |
| void | **[SetInitializationCriterion](../Classes/classshapeworks_1_1Optimize.md#function-setinitializationcriterion)**(double init_criterion)<br>Set the init criterion (TODO: details)  |
| void | **[SetOptimizationCriterion](../Classes/classshapeworks_1_1Optimize.md#function-setoptimizationcriterion)**(double opt_criterion)<br>Set the optimization criterion (TODO: details)  |
| void | **[SetUseShapeStatisticsInInit](../Classes/classshapeworks_1_1Optimize.md#function-setuseshapestatisticsininit)**(bool use_shape_statistics_in_init)<br>Set if shape statistics should be used in initialization.  |
| void | **[SetProcrustesInterval](../Classes/classshapeworks_1_1Optimize.md#function-setprocrustesinterval)**(int procrustes_interval)<br>Set the interval for running procrustes (0 to disable)  |
| void | **[SetProcrustesScaling](../Classes/classshapeworks_1_1Optimize.md#function-setprocrustesscaling)**(int procrustes_scaling)<br>Set if procrustes scaling should be used (0=disabled, 1=enabled)  |
| void | **[SetRelativeWeighting](../Classes/classshapeworks_1_1Optimize.md#function-setrelativeweighting)**(double relative_weighting)<br>Set the relative weighting (TODO: details)  |
| void | **[SetInitialRelativeWeighting](../Classes/classshapeworks_1_1Optimize.md#function-setinitialrelativeweighting)**(double initial_relative_weighting)<br>Set the initial relative weigting (TODO: details)  |
| void | **[SetStartingRegularization](../Classes/classshapeworks_1_1Optimize.md#function-setstartingregularization)**(double starting_regularization)<br>Set the starting regularization (TODO: details)  |
| void | **[SetEndingRegularization](../Classes/classshapeworks_1_1Optimize.md#function-setendingregularization)**(double ending_regularization)<br>Set the ending regularization (TODO: details)  |
| void | **[SetRecomputeRegularizationInterval](../Classes/classshapeworks_1_1Optimize.md#function-setrecomputeregularizationinterval)**(int recompute_regularization_interval)<br>Set the interval for recomputing regularization (TODO: details)  |
| void | **[SetSaveInitSplits](../Classes/classshapeworks_1_1Optimize.md#function-setsaveinitsplits)**(bool save_init_splits)<br>Set if initialization splits should be saved or not.  |
| void | **[SetCheckpointingInterval](../Classes/classshapeworks_1_1Optimize.md#function-setcheckpointinginterval)**(int checkpointing_interval)<br>Set the checkpointing interval.  |
| void | **[SetKeepCheckpoints](../Classes/classshapeworks_1_1Optimize.md#function-setkeepcheckpoints)**(int keep_checkpoints)<br>Set if checkpoints should be kept (0=disable, 1=enable)  |
| void | **[SetCotanSigmaFactor](../Classes/classshapeworks_1_1Optimize.md#function-setcotansigmafactor)**(double cotan_sigma_factor)<br>Set the cotan sigma factor (TODO: details)  |
| void | **[SetUseRegression](../Classes/classshapeworks_1_1Optimize.md#function-setuseregression)**(bool use_regression)<br>Set if regression should be used (TODO: details)  |
| void | **[SetUseMixedEffects](../Classes/classshapeworks_1_1Optimize.md#function-setusemixedeffects)**(bool use_mixed_effects)<br>Set if mixed effects should be used (TODO: details)  |
| void | **[SetNormalAngle](../Classes/classshapeworks_1_1Optimize.md#function-setnormalangle)**(double normal_angle)<br>For good/bad analysis, set the normal angle to use (TODO: details)  |
| void | **[SetPerformGoodBad](../Classes/classshapeworks_1_1Optimize.md#function-setperformgoodbad)**(bool perform_good_bad)<br>Set if good/bad analysis should be done (TODO: details)  |
| void | **[SetLogEnergy](../Classes/classshapeworks_1_1Optimize.md#function-setlogenergy)**(bool log_energy)<br>Set the log energy (TODO: details)  |
| void | **[AddImage](../Classes/classshapeworks_1_1Optimize.md#function-addimage)**(ImageType::Pointer image, std::string name ="")<br>Set the shape input images.  |
| void | **[AddMesh](../Classes/classshapeworks_1_1Optimize.md#function-addmesh)**(vtkSmartPointer< vtkPolyData > poly_data) |
| void | **[AddContour](../Classes/classshapeworks_1_1Optimize.md#function-addcontour)**(vtkSmartPointer< vtkPolyData > poly_data) |
| void | **[SetFilenames](../Classes/classshapeworks_1_1Optimize.md#function-setfilenames)**(const std::vector< std::string > & filenames)<br>Set the shape filenames (TODO: details)  |
| void | **[SetPointFiles](../Classes/classshapeworks_1_1Optimize.md#function-setpointfiles)**(const std::vector< std::string > & point_files)<br>Set starting point files (TODO: details)  |
| int | **[GetNumShapes](../Classes/classshapeworks_1_1Optimize.md#function-getnumshapes)**()<br>Get number of shapes.  |
| void | **[SetMeshFiles](../Classes/classshapeworks_1_1Optimize.md#function-setmeshfiles)**(const std::vector< std::string > & mesh_files)<br>Set the mesh files (TODO: details)  |
| void | **[SetAttributeScales](../Classes/classshapeworks_1_1Optimize.md#function-setattributescales)**(const std::vector< double > & scales)<br>Set attribute scales (TODO: details)  |
| void | **[SetFeaFiles](../Classes/classshapeworks_1_1Optimize.md#function-setfeafiles)**(const std::vector< std::string > & files)<br>Set FEA files (TODO: details)  |
| void | **[SetFeaGradFiles](../Classes/classshapeworks_1_1Optimize.md#function-setfeagradfiles)**(const std::vector< std::string > & files)<br>Set FEA grad files (TODO: details)  |
| void | **[SetFidsFiles](../Classes/classshapeworks_1_1Optimize.md#function-setfidsfiles)**(const std::vector< std::string > & files)<br>Set FIDS files (TODO: details)  |
| void | **[SetParticleFlags](../Classes/classshapeworks_1_1Optimize.md#function-setparticleflags)**(std::vector< int > flags)<br>Set Particle Flags (TODO: details)  |
| void | **[SetDomainFlags](../Classes/classshapeworks_1_1Optimize.md#function-setdomainflags)**(std::vector< int > flags)<br>Set Domain Flags (TODO: details)  |
| const std::vector< int > & | **[GetDomainFlags](../Classes/classshapeworks_1_1Optimize.md#function-getdomainflags)**() |
| void | **[SetFileOutputEnabled](../Classes/classshapeworks_1_1Optimize.md#function-setfileoutputenabled)**(bool enabled)<br>Set if file output is enabled.  |
| std::vector< bool > | **[GetUseXYZ](../Classes/classshapeworks_1_1Optimize.md#function-getusexyz)**()<br>Return if XYZ is used, per shape.  |
| std::vector< bool > | **[GetUseNormals](../Classes/classshapeworks_1_1Optimize.md#function-getusenormals)**()<br>Return if Normals are used, per shape.  |
| void | **[SetNarrowBand](../Classes/classshapeworks_1_1Optimize.md#function-setnarrowband)**(double v)<br>Set the narrow band used to be +/- the given value as a multiple of the spacing.  |
| double | **[GetNarrowBand](../Classes/classshapeworks_1_1Optimize.md#function-getnarrowband)**()<br>Return the narrow band to be used.  |
| void | **[SetUseShapeStatisticsAfter](../Classes/classshapeworks_1_1Optimize.md#function-setuseshapestatisticsafter)**(int num_particles)<br>Set the number of particles when correspondence based multiscale takes over.  |
| int | **[GetUseShapeStatisticsAfter](../Classes/classshapeworks_1_1Optimize.md#function-getuseshapestatisticsafter)**()<br>Return the number of particles when correspondence based multiscale takes over.  |
| void | **[PrintParamInfo](../Classes/classshapeworks_1_1Optimize.md#function-printparaminfo)**()<br>Print parameter info to stdout.  |
| std::shared_ptr< [Sampler](../Classes/classshapeworks_1_1Sampler.md) > | **[GetSampler](../Classes/classshapeworks_1_1Optimize.md#function-getsampler)**()<br>Return the [Sampler]().  |
| [MatrixContainer](../Classes/classshapeworks_1_1MatrixContainer.md) | **[GetParticleSystem](../Classes/classshapeworks_1_1Optimize.md#function-getparticlesystem)**()<br>Return the particle system as a matrix.  |
| void | **[SetPythonFile](../Classes/classshapeworks_1_1Optimize.md#function-setpythonfile)**(std::string filename)<br>Set the python file to run at startup.  |
| void | **[SetGeodesicsEnabled](../Classes/classshapeworks_1_1Optimize.md#function-setgeodesicsenabled)**(bool is_enabled)<br>Set whether or not geodesics are enabled.  |
| void | **[SetGeodesicsCacheSizeMultiplier](../Classes/classshapeworks_1_1Optimize.md#function-setgeodesicscachesizemultiplier)**(size_t n) |
| [shapeworks::OptimizationVisualizer](../Classes/classshapeworks_1_1OptimizationVisualizer.md) & | **[GetVisualizer](../Classes/classshapeworks_1_1Optimize.md#function-getvisualizer)**() |
| void | **[SetShowVisualizer](../Classes/classshapeworks_1_1Optimize.md#function-setshowvisualizer)**(bool show) |
| bool | **[GetShowVisualizer](../Classes/classshapeworks_1_1Optimize.md#function-getshowvisualizer)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| virtual void | **[SetIterationCallback](../Classes/classshapeworks_1_1Optimize.md#function-setiterationcallback)**()<br>Set the iteration callback. Derived classes should override to set their own callback.  |
| void | **[RunProcrustes](../Classes/classshapeworks_1_1Optimize.md#function-runprocrustes)**()<br>Run an iteration of procrustes.  |
| void | **[OptimizeStart](../Classes/classshapeworks_1_1Optimize.md#function-optimizestart)**() |
| void | **[OptimizerStop](../Classes/classshapeworks_1_1Optimize.md#function-optimizerstop)**() |
| void | **[ReadTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-readtransformfile)**() |
| void | **[ReadPrefixTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-readprefixtransformfile)**(const std::string & s) |
| void | **[InitializeSampler](../Classes/classshapeworks_1_1Optimize.md#function-initializesampler)**() |
| double | **[GetMinNeighborhoodRadius](../Classes/classshapeworks_1_1Optimize.md#function-getminneighborhoodradius)**() |
| void | **[AddSinglePoint](../Classes/classshapeworks_1_1Optimize.md#function-addsinglepoint)**() |
| void | **[Initialize](../Classes/classshapeworks_1_1Optimize.md#function-initialize)**() |
| void | **[AddAdaptivity](../Classes/classshapeworks_1_1Optimize.md#function-addadaptivity)**() |
| void | **[RunOptimize](../Classes/classshapeworks_1_1Optimize.md#function-runoptimize)**() |
| void | **[SetInitialCorrespondenceMode](../Classes/classshapeworks_1_1Optimize.md#function-setinitialcorrespondencemode)**() |
| virtual void | **[IterateCallback](../Classes/classshapeworks_1_1Optimize.md#function-iteratecallback)**(itk::Object * , const itk::EventObject & ) |
| void | **[ComputeEnergyAfterIteration](../Classes/classshapeworks_1_1Optimize.md#function-computeenergyafteriteration)**() |
| void | **[SetCotanSigma](../Classes/classshapeworks_1_1Optimize.md#function-setcotansigma)**() |
| void | **[WriteTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-writetransformfile)**(int iter =-1) const |
| void | **[WriteTransformFile](../Classes/classshapeworks_1_1Optimize.md#function-writetransformfile)**(std::string iter_prefix) const |
| void | **[WriteTransformFiles](../Classes/classshapeworks_1_1Optimize.md#function-writetransformfiles)**(int iter =-1) const |
| void | **[WriteTransformFiles](../Classes/classshapeworks_1_1Optimize.md#function-writetransformfiles)**(std::string iter_prefix) const |
| void | **[WritePointFiles](../Classes/classshapeworks_1_1Optimize.md#function-writepointfiles)**(int iter =-1) |
| void | **[WritePointFiles](../Classes/classshapeworks_1_1Optimize.md#function-writepointfiles)**(std::string iter_prefix) |
| void | **[WritePointFilesWithFeatures](../Classes/classshapeworks_1_1Optimize.md#function-writepointfileswithfeatures)**(int iter =-1) |
| void | **[WritePointFilesWithFeatures](../Classes/classshapeworks_1_1Optimize.md#function-writepointfileswithfeatures)**(std::string iter_prefix) |
| void | **[WriteEnergyFiles](../Classes/classshapeworks_1_1Optimize.md#function-writeenergyfiles)**() |
| void | **[WriteCuttingPlanePoints](../Classes/classshapeworks_1_1Optimize.md#function-writecuttingplanepoints)**(int iter =-1) |
| void | **[WriteParameters](../Classes/classshapeworks_1_1Optimize.md#function-writeparameters)**(std::string output_dir ="") |
| void | **[ReportBadParticles](../Classes/classshapeworks_1_1Optimize.md#function-reportbadparticles)**() |
| void | **[SetParameters](../Classes/classshapeworks_1_1Optimize.md#function-setparameters)**() |
| void | **[WriteModes](../Classes/classshapeworks_1_1Optimize.md#function-writemodes)**() |
| void | **[PrintStartMessage](../Classes/classshapeworks_1_1Optimize.md#function-printstartmessage)**(std::string str, unsigned int vlevel =0) const |
| void | **[PrintDoneMessage](../Classes/classshapeworks_1_1Optimize.md#function-printdonemessage)**(unsigned int vlevel =0) const |
| virtual void | **[UpdateExportablePoints](../Classes/classshapeworks_1_1Optimize.md#function-updateexportablepoints)**() |
| std::string | **[GetCheckpointDir](../Classes/classshapeworks_1_1Optimize.md#function-getcheckpointdir)**() |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| std::shared_ptr< [Sampler](../Classes/classshapeworks_1_1Sampler.md) > | **[m_sampler](../Classes/classshapeworks_1_1Optimize.md#variable-m-sampler)**  |
| [itk::ParticleProcrustesRegistration](../Classes/classitk_1_1ParticleProcrustesRegistration.md)< 3 >::Pointer | **[m_procrustes](../Classes/classshapeworks_1_1Optimize.md#variable-m-procrustes)**  |
| [itk::ParticleGoodBadAssessment](../Classes/classitk_1_1ParticleGoodBadAssessment.md)< float, 3 >::Pointer | **[m_good_bad](../Classes/classshapeworks_1_1Optimize.md#variable-m-good-bad)**  |
| unsigned int | **[m_verbosity_level](../Classes/classshapeworks_1_1Optimize.md#variable-m-verbosity-level)**  |
| std::vector< std::vector< itk::Point< double > > > | **[m_local_points](../Classes/classshapeworks_1_1Optimize.md#variable-m-local-points)**  |
| std::vector< std::vector< itk::Point< double > > > | **[m_global_points](../Classes/classshapeworks_1_1Optimize.md#variable-m-global-points)**  |
| int | **[m_checkpoint_counter](../Classes/classshapeworks_1_1Optimize.md#variable-m-checkpoint-counter)**  |
| int | **[m_procrustes_counter](../Classes/classshapeworks_1_1Optimize.md#variable-m-procrustes-counter)**  |
| int | **[m_saturation_counter](../Classes/classshapeworks_1_1Optimize.md#variable-m-saturation-counter)**  |
| bool | **[m_disable_procrustes](../Classes/classshapeworks_1_1Optimize.md#variable-m-disable-procrustes)**  |
| bool | **[m_use_cutting_planes](../Classes/classshapeworks_1_1Optimize.md#variable-m-use-cutting-planes)**  |
| bool | **[m_optimizing](../Classes/classshapeworks_1_1Optimize.md#variable-m-optimizing)**  |
| bool | **[m_use_regression](../Classes/classshapeworks_1_1Optimize.md#variable-m-use-regression)**  |
| bool | **[m_use_mixed_effects](../Classes/classshapeworks_1_1Optimize.md#variable-m-use-mixed-effects)**  |
| unsigned int | **[m_domains_per_shape](../Classes/classshapeworks_1_1Optimize.md#variable-m-domains-per-shape)**  |
| shapeworks::DomainType | **[m_domain_type](../Classes/classshapeworks_1_1Optimize.md#variable-m-domain-type)**  |
| std::vector< int > | **[m_number_of_particles](../Classes/classshapeworks_1_1Optimize.md#variable-m-number-of-particles)**  |
| std::string | **[m_transform_file](../Classes/classshapeworks_1_1Optimize.md#variable-m-transform-file)**  |
| std::string | **[m_prefix_transform_file](../Classes/classshapeworks_1_1Optimize.md#variable-m-prefix-transform-file)**  |
| std::string | **[m_output_dir](../Classes/classshapeworks_1_1Optimize.md#variable-m-output-dir)**  |
| std::string | **[m_output_transform_file](../Classes/classshapeworks_1_1Optimize.md#variable-m-output-transform-file)**  |
| bool | **[m_output_transform_files](../Classes/classshapeworks_1_1Optimize.md#variable-m-output-transform-files)**  |
| bool | **[m_mesh_based_attributes](../Classes/classshapeworks_1_1Optimize.md#variable-m-mesh-based-attributes)**  |
| std::vector< bool > | **[m_use_xyz](../Classes/classshapeworks_1_1Optimize.md#variable-m-use-xyz)**  |
| std::vector< bool > | **[m_use_normals](../Classes/classshapeworks_1_1Optimize.md#variable-m-use-normals)**  |
| std::vector< int > | **[m_attributes_per_domain](../Classes/classshapeworks_1_1Optimize.md#variable-m-attributes-per-domain)**  |
| int | **[m_distribution_domain_id](../Classes/classshapeworks_1_1Optimize.md#variable-m-distribution-domain-id)**  |
| std::string | **[m_output_cutting_plane_file](../Classes/classshapeworks_1_1Optimize.md#variable-m-output-cutting-plane-file)**  |
| int | **[m_processing_mode](../Classes/classshapeworks_1_1Optimize.md#variable-m-processing-mode)**  |
| int | **[m_adaptivity_mode](../Classes/classshapeworks_1_1Optimize.md#variable-m-adaptivity-mode)**  |
| double | **[m_adaptivity_strength](../Classes/classshapeworks_1_1Optimize.md#variable-m-adaptivity-strength)**  |
| int | **[m_pairwise_potential_type](../Classes/classshapeworks_1_1Optimize.md#variable-m-pairwise-potential-type)**  |
| unsigned int | **[m_timepts_per_subject](../Classes/classshapeworks_1_1Optimize.md#variable-m-timepts-per-subject)**  |
| int | **[m_optimization_iterations](../Classes/classshapeworks_1_1Optimize.md#variable-m-optimization-iterations)**  |
| int | **[m_optimization_iterations_completed](../Classes/classshapeworks_1_1Optimize.md#variable-m-optimization-iterations-completed)**  |
| int | **[m_iterations_per_split](../Classes/classshapeworks_1_1Optimize.md#variable-m-iterations-per-split)**  |
| double | **[m_initialization_criterion](../Classes/classshapeworks_1_1Optimize.md#variable-m-initialization-criterion)**  |
| double | **[m_optimization_criterion](../Classes/classshapeworks_1_1Optimize.md#variable-m-optimization-criterion)**  |
| bool | **[m_use_shape_statistics_in_init](../Classes/classshapeworks_1_1Optimize.md#variable-m-use-shape-statistics-in-init)**  |
| unsigned int | **[m_procrustes_interval](../Classes/classshapeworks_1_1Optimize.md#variable-m-procrustes-interval)**  |
| int | **[m_procrustes_scaling](../Classes/classshapeworks_1_1Optimize.md#variable-m-procrustes-scaling)**  |
| double | **[m_relative_weighting](../Classes/classshapeworks_1_1Optimize.md#variable-m-relative-weighting)**  |
| double | **[m_initial_relative_weighting](../Classes/classshapeworks_1_1Optimize.md#variable-m-initial-relative-weighting)**  |
| double | **[m_starting_regularization](../Classes/classshapeworks_1_1Optimize.md#variable-m-starting-regularization)**  |
| double | **[m_ending_regularization](../Classes/classshapeworks_1_1Optimize.md#variable-m-ending-regularization)**  |
| int | **[m_recompute_regularization_interval](../Classes/classshapeworks_1_1Optimize.md#variable-m-recompute-regularization-interval)**  |
| bool | **[m_save_init_splits](../Classes/classshapeworks_1_1Optimize.md#variable-m-save-init-splits)**  |
| unsigned int | **[m_checkpointing_interval](../Classes/classshapeworks_1_1Optimize.md#variable-m-checkpointing-interval)**  |
| int | **[m_keep_checkpoints](../Classes/classshapeworks_1_1Optimize.md#variable-m-keep-checkpoints)**  |
| double | **[m_cotan_sigma_factor](../Classes/classshapeworks_1_1Optimize.md#variable-m-cotan-sigma-factor)**  |
| std::vector< int > | **[m_particle_flags](../Classes/classshapeworks_1_1Optimize.md#variable-m-particle-flags)**  |
| std::vector< int > | **[m_domain_flags](../Classes/classshapeworks_1_1Optimize.md#variable-m-domain-flags)**  |
| double | **[m_narrow_band](../Classes/classshapeworks_1_1Optimize.md#variable-m-narrow-band)**  |
| bool | **[m_narrow_band_set](../Classes/classshapeworks_1_1Optimize.md#variable-m-narrow-band-set)**  |
| bool | **[m_fixed_domains_present](../Classes/classshapeworks_1_1Optimize.md#variable-m-fixed-domains-present)**  |
| int | **[m_use_shape_statistics_after](../Classes/classshapeworks_1_1Optimize.md#variable-m-use-shape-statistics-after)**  |
| std::string | **[m_python_filename](../Classes/classshapeworks_1_1Optimize.md#variable-m-python-filename)**  |
| bool | **[m_geodesics_enabled](../Classes/classshapeworks_1_1Optimize.md#variable-m-geodesics-enabled)**  |
| size_t | **[m_geodesic_cache_size_multiplier](../Classes/classshapeworks_1_1Optimize.md#variable-m-geodesic-cache-size-multiplier)**  |
| unsigned int | **[m_mode](../Classes/classshapeworks_1_1Optimize.md#variable-m-mode)**  |
| double | **[m_spacing](../Classes/classshapeworks_1_1Optimize.md#variable-m-spacing)**  |
| std::vector< std::string > | **[m_filenames](../Classes/classshapeworks_1_1Optimize.md#variable-m-filenames)**  |
| int | **[m_num_shapes](../Classes/classshapeworks_1_1Optimize.md#variable-m-num-shapes)**  |
| std::vector< double > | **[m_energy_a](../Classes/classshapeworks_1_1Optimize.md#variable-m-energy-a)**  |
| std::vector< double > | **[m_energy_b](../Classes/classshapeworks_1_1Optimize.md#variable-m-energy-b)**  |
| std::vector< double > | **[m_total_energy](../Classes/classshapeworks_1_1Optimize.md#variable-m-total-energy)**  |
| bool | **[m_log_energy](../Classes/classshapeworks_1_1Optimize.md#variable-m-log-energy)**  |
| std::string | **[m_str_energy](../Classes/classshapeworks_1_1Optimize.md#variable-m-str-energy)**  |
| std::vector< std::vector< int > > | **[m_bad_ids](../Classes/classshapeworks_1_1Optimize.md#variable-m-bad-ids)**  |
| double | **[m_normal_angle](../Classes/classshapeworks_1_1Optimize.md#variable-m-normal-angle)**  |
| bool | **[m_perform_good_bad](../Classes/classshapeworks_1_1Optimize.md#variable-m-perform-good-bad)**  |
| std::vector< int > | **[m_cutting_planes_per_input](../Classes/classshapeworks_1_1Optimize.md#variable-m-cutting-planes-per-input)**  |
| std::vector< int > | **[m_spheres_per_input](../Classes/classshapeworks_1_1Optimize.md#variable-m-spheres-per-input)**  |
| bool | **[m_file_output_enabled](../Classes/classshapeworks_1_1Optimize.md#variable-m-file-output-enabled)**  |
| bool | **[m_aborted](../Classes/classshapeworks_1_1Optimize.md#variable-m-aborted)**  |
| std::vector< std::array< itk::Point< double >, 3 > > | **[m_cut_planes](../Classes/classshapeworks_1_1Optimize.md#variable-m-cut-planes)**  |
| int | **[m_total_iterations](../Classes/classshapeworks_1_1Optimize.md#variable-m-total-iterations)**  |
| int | **[m_iteration_count](../Classes/classshapeworks_1_1Optimize.md#variable-m-iteration-count)**  |
| int | **[m_split_number](../Classes/classshapeworks_1_1Optimize.md#variable-m-split-number)**  |
| std::mt19937 | **[m_rand](../Classes/classshapeworks_1_1Optimize.md#variable-m-rand)**  |
| std::function< void(void)> | **[m_iter_callback](../Classes/classshapeworks_1_1Optimize.md#variable-m-iter-callback)**  |
| bool | **[show_visualizer](../Classes/classshapeworks_1_1Optimize.md#variable-show-visualizer)**  |
| [shapeworks::OptimizationVisualizer](../Classes/classshapeworks_1_1OptimizationVisualizer.md) | **[visualizer](../Classes/classshapeworks_1_1Optimize.md#variable-visualizer)**  |

## Detailed Description

```cpp
class shapeworks::Optimize;
```


**Par**: Overview of Optimize

This class is the top-level API for the optimization library


The [Optimize](../Classes/classshapeworks_1_1Optimize.md) class controls all aspect of the particle system library provides a top-level public API for running optimization 

## Public Types Documentation

### using ImageType

```cpp
using shapeworks::Optimize::ImageType =  itk::Image<float, 3>;
```


### using VectorType

```cpp
using shapeworks::Optimize::VectorType =  itk::ParticleVectorFunction<3>::VectorType;
```


### using MatrixType

```cpp
using shapeworks::Optimize::MatrixType =  Eigen::MatrixXd;
```


## Public Functions Documentation

### function Optimize

```cpp
Optimize()
```

Constructor. 

### function ~Optimize

```cpp
virtual ~Optimize()
```

Destructor. 

### function Run

```cpp
bool Run()
```

Run the optimization. 

### function LoadParameterFile

```cpp
bool LoadParameterFile(
    std::string filename
)
```

Load a parameter file. 

### function SetIterationCallbackFunction

```cpp
inline void SetIterationCallbackFunction(
    const std::function< void(void)> & f
)
```


### function AbortOptimization

```cpp
void AbortOptimization()
```

Abort optimization. 

### function GetAborted

```cpp
bool GetAborted()
```

Return if the optimization was aborted. 

### function GetLocalPoints

```cpp
virtual std::vector< std::vector< itk::Point< double > > > GetLocalPoints()
```

Return the local points. 

**Reimplemented by**: [shapeworks::QOptimize::GetLocalPoints](../Classes/classshapeworks_1_1QOptimize.md#function-getlocalpoints)


### function GetGlobalPoints

```cpp
virtual std::vector< std::vector< itk::Point< double > > > GetGlobalPoints()
```

Return the global points. 

**Reimplemented by**: [shapeworks::QOptimize::GetGlobalPoints](../Classes/classshapeworks_1_1QOptimize.md#function-getglobalpoints)


### function SetCutPlanes

```cpp
void SetCutPlanes(
    std::vector< std::array< itk::Point< double >, 3 >> cut_planes
)
```

Set cutting planes. 

### function SetVerbosity

```cpp
void SetVerbosity(
    int verbosity_level
)
```

Set the verbosity level (0-5) 

### function SetDomainsPerShape

```cpp
void SetDomainsPerShape(
    int domains_per_shape
)
```

Set the number of domains per shape. 

### function GetDomainsPerShape

```cpp
int GetDomainsPerShape()
```

Return the number of domains per shape. 

### function SetDomainType

```cpp
void SetDomainType(
    shapeworks::DomainType type
)
```


### function GetDomainType

```cpp
shapeworks::DomainType GetDomainType()
```


### function SetNumberOfParticles

```cpp
void SetNumberOfParticles(
    std::vector< int > number_of_particles
)
```

Set the numbers of particles (vector of numbers, one for each domain) 

### function GetNumberOfParticles

```cpp
std::vector< int > GetNumberOfParticles()
```

Return the numbers of particles per domain. 

### function SetTransformFile

```cpp
void SetTransformFile(
    std::string filename
)
```

Set the transform file. 

### function GetTransformFile

```cpp
std::string GetTransformFile()
```

Get the transform file. 

### function SetPrefixTransformFile

```cpp
void SetPrefixTransformFile(
    std::string prefix_transform_file
)
```

Set the prefix transform file (TODO: details) 

### function GetPrefixTransformFile

```cpp
std::string GetPrefixTransformFile()
```

Get the prefix transform file. 

### function SetOutputDir

```cpp
void SetOutputDir(
    std::string output_dir
)
```

Set the output directory. 

### function SetOutputTransformFile

```cpp
void SetOutputTransformFile(
    std::string output_transform_file
)
```

Set the output transform file. 

### function SetOutputIndividualTransformFiles

```cpp
void SetOutputIndividualTransformFiles(
    bool value
)
```

Set whether individual transforms should be written. 

### function SetUseMeshBasedAttributes

```cpp
void SetUseMeshBasedAttributes(
    bool use_mesh_based_attributes
)
```

Set if mesh based attributes should be used. 

### function GetUseMeshBasedAttributes

```cpp
bool GetUseMeshBasedAttributes()
```

Get if mesh based attributes are being used. 

### function SetUseXYZ

```cpp
void SetUseXYZ(
    std::vector< bool > use_xyz
)
```

Set if XYZ is used, one value per domain. 

### function SetUseNormals

```cpp
void SetUseNormals(
    std::vector< bool > use_normals
)
```

Set if Normals are being used, one value per domain. 

### function SetAttributesPerDomain

```cpp
void SetAttributesPerDomain(
    std::vector< int > attributes_per_domain
)
```

Set the attributes per domain. 

### function GetAttributesPerDomain

```cpp
std::vector< int > GetAttributesPerDomain()
```

Get attributes per domain. 

### function SetDistributionDomainID

```cpp
void SetDistributionDomainID(
    int distribution_domain_id
)
```

Set the distribution domain ID (TODO: details) 

### function GetDistributionDomainID

```cpp
int GetDistributionDomainID()
```

Get the distribution domain ID. 

### function SetOutputCuttingPlaneFile

```cpp
void SetOutputCuttingPlaneFile(
    std::string output_cutting_plane_file
)
```

Set the output cutting plane file. 

### function SetUseCuttingPlanes

```cpp
void SetUseCuttingPlanes(
    bool use_cutting_planes
)
```

Set if using cutting planes. 

### function SetCuttingPlane

```cpp
void SetCuttingPlane(
    unsigned int i,
    const vnl_vector_fixed< double, 3 > & va,
    const vnl_vector_fixed< double, 3 > & vb,
    const vnl_vector_fixed< double, 3 > & vc
)
```

Set a given cutting plane for a shape. 

### function SetProcessingMode

```cpp
void SetProcessingMode(
    int mode
)
```

Set processing mode (TODO: details) 

### function SetAdaptivityMode

```cpp
void SetAdaptivityMode(
    int adaptivity_mode
)
```

Set adaptivity mode (TODO: details) 

### function SetAdaptivityStrength

```cpp
void SetAdaptivityStrength(
    double adaptivity_strength
)
```

Set adaptivity strength (TODO: details) 

### function SetPairwisePotentialType

```cpp
void SetPairwisePotentialType(
    int pairwise_potential_type
)
```

Set pairwise potential type (TODO: details) 

### function SetTimePtsPerSubject

```cpp
void SetTimePtsPerSubject(
    int time_pts_per_subject
)
```

Set the number of time points per subject (TODO: details) 

### function GetTimePtsPerSubject

```cpp
int GetTimePtsPerSubject()
```

Get the number of time points per subject (TODO: details) 

### function SetOptimizationIterations

```cpp
void SetOptimizationIterations(
    int optimization_iterations
)
```

Set the number of optimization iterations. 

### function SetOptimizationIterationsCompleted

```cpp
void SetOptimizationIterationsCompleted(
    int optimization_iterations_completed
)
```

Set the number of optimization iterations already completed (TODO: details) 

### function SetIterationsPerSplit

```cpp
void SetIterationsPerSplit(
    int iterations_per_split
)
```

Set the number of iterations per split. 

### function SetInitializationCriterion

```cpp
void SetInitializationCriterion(
    double init_criterion
)
```

Set the init criterion (TODO: details) 

### function SetOptimizationCriterion

```cpp
void SetOptimizationCriterion(
    double opt_criterion
)
```

Set the optimization criterion (TODO: details) 

### function SetUseShapeStatisticsInInit

```cpp
void SetUseShapeStatisticsInInit(
    bool use_shape_statistics_in_init
)
```

Set if shape statistics should be used in initialization. 

### function SetProcrustesInterval

```cpp
void SetProcrustesInterval(
    int procrustes_interval
)
```

Set the interval for running procrustes (0 to disable) 

### function SetProcrustesScaling

```cpp
void SetProcrustesScaling(
    int procrustes_scaling
)
```

Set if procrustes scaling should be used (0=disabled, 1=enabled) 

### function SetRelativeWeighting

```cpp
void SetRelativeWeighting(
    double relative_weighting
)
```

Set the relative weighting (TODO: details) 

### function SetInitialRelativeWeighting

```cpp
void SetInitialRelativeWeighting(
    double initial_relative_weighting
)
```

Set the initial relative weigting (TODO: details) 

### function SetStartingRegularization

```cpp
void SetStartingRegularization(
    double starting_regularization
)
```

Set the starting regularization (TODO: details) 

### function SetEndingRegularization

```cpp
void SetEndingRegularization(
    double ending_regularization
)
```

Set the ending regularization (TODO: details) 

### function SetRecomputeRegularizationInterval

```cpp
void SetRecomputeRegularizationInterval(
    int recompute_regularization_interval
)
```

Set the interval for recomputing regularization (TODO: details) 

### function SetSaveInitSplits

```cpp
void SetSaveInitSplits(
    bool save_init_splits
)
```

Set if initialization splits should be saved or not. 

### function SetCheckpointingInterval

```cpp
void SetCheckpointingInterval(
    int checkpointing_interval
)
```

Set the checkpointing interval. 

### function SetKeepCheckpoints

```cpp
void SetKeepCheckpoints(
    int keep_checkpoints
)
```

Set if checkpoints should be kept (0=disable, 1=enable) 

### function SetCotanSigmaFactor

```cpp
void SetCotanSigmaFactor(
    double cotan_sigma_factor
)
```

Set the cotan sigma factor (TODO: details) 

### function SetUseRegression

```cpp
void SetUseRegression(
    bool use_regression
)
```

Set if regression should be used (TODO: details) 

### function SetUseMixedEffects

```cpp
void SetUseMixedEffects(
    bool use_mixed_effects
)
```

Set if mixed effects should be used (TODO: details) 

### function SetNormalAngle

```cpp
void SetNormalAngle(
    double normal_angle
)
```

For good/bad analysis, set the normal angle to use (TODO: details) 

### function SetPerformGoodBad

```cpp
void SetPerformGoodBad(
    bool perform_good_bad
)
```

Set if good/bad analysis should be done (TODO: details) 

### function SetLogEnergy

```cpp
void SetLogEnergy(
    bool log_energy
)
```

Set the log energy (TODO: details) 

### function AddImage

```cpp
void AddImage(
    ImageType::Pointer image,
    std::string name =""
)
```

Set the shape input images. 

### function AddMesh

```cpp
void AddMesh(
    vtkSmartPointer< vtkPolyData > poly_data
)
```


### function AddContour

```cpp
void AddContour(
    vtkSmartPointer< vtkPolyData > poly_data
)
```


### function SetFilenames

```cpp
void SetFilenames(
    const std::vector< std::string > & filenames
)
```

Set the shape filenames (TODO: details) 

### function SetPointFiles

```cpp
void SetPointFiles(
    const std::vector< std::string > & point_files
)
```

Set starting point files (TODO: details) 

### function GetNumShapes

```cpp
int GetNumShapes()
```

Get number of shapes. 

### function SetMeshFiles

```cpp
void SetMeshFiles(
    const std::vector< std::string > & mesh_files
)
```

Set the mesh files (TODO: details) 

### function SetAttributeScales

```cpp
void SetAttributeScales(
    const std::vector< double > & scales
)
```

Set attribute scales (TODO: details) 

### function SetFeaFiles

```cpp
void SetFeaFiles(
    const std::vector< std::string > & files
)
```

Set FEA files (TODO: details) 

### function SetFeaGradFiles

```cpp
void SetFeaGradFiles(
    const std::vector< std::string > & files
)
```

Set FEA grad files (TODO: details) 

### function SetFidsFiles

```cpp
void SetFidsFiles(
    const std::vector< std::string > & files
)
```

Set FIDS files (TODO: details) 

### function SetParticleFlags

```cpp
void SetParticleFlags(
    std::vector< int > flags
)
```

Set Particle Flags (TODO: details) 

### function SetDomainFlags

```cpp
void SetDomainFlags(
    std::vector< int > flags
)
```

Set Domain Flags (TODO: details) 

### function GetDomainFlags

```cpp
const std::vector< int > & GetDomainFlags()
```


### function SetFileOutputEnabled

```cpp
void SetFileOutputEnabled(
    bool enabled
)
```

Set if file output is enabled. 

### function GetUseXYZ

```cpp
std::vector< bool > GetUseXYZ()
```

Return if XYZ is used, per shape. 

### function GetUseNormals

```cpp
std::vector< bool > GetUseNormals()
```

Return if Normals are used, per shape. 

### function SetNarrowBand

```cpp
void SetNarrowBand(
    double v
)
```

Set the narrow band used to be +/- the given value as a multiple of the spacing. 

### function GetNarrowBand

```cpp
double GetNarrowBand()
```

Return the narrow band to be used. 

### function SetUseShapeStatisticsAfter

```cpp
void SetUseShapeStatisticsAfter(
    int num_particles
)
```

Set the number of particles when correspondence based multiscale takes over. 

### function GetUseShapeStatisticsAfter

```cpp
int GetUseShapeStatisticsAfter()
```

Return the number of particles when correspondence based multiscale takes over. 

### function PrintParamInfo

```cpp
void PrintParamInfo()
```

Print parameter info to stdout. 

### function GetSampler

```cpp
inline std::shared_ptr< Sampler > GetSampler()
```

Return the [Sampler](). 

### function GetParticleSystem

```cpp
MatrixContainer GetParticleSystem()
```

Return the particle system as a matrix. 

### function SetPythonFile

```cpp
void SetPythonFile(
    std::string filename
)
```

Set the python file to run at startup. 

### function SetGeodesicsEnabled

```cpp
void SetGeodesicsEnabled(
    bool is_enabled
)
```

Set whether or not geodesics are enabled. 

### function SetGeodesicsCacheSizeMultiplier

```cpp
void SetGeodesicsCacheSizeMultiplier(
    size_t n
)
```


Set cache size multiplier for geodesics. The total number of cache entries will be n * number_of_triangles 


### function GetVisualizer

```cpp
shapeworks::OptimizationVisualizer & GetVisualizer()
```


### function SetShowVisualizer

```cpp
void SetShowVisualizer(
    bool show
)
```


### function GetShowVisualizer

```cpp
bool GetShowVisualizer()
```


## Protected Functions Documentation

### function SetIterationCallback

```cpp
virtual void SetIterationCallback()
```

Set the iteration callback. Derived classes should override to set their own callback. 

**Reimplemented by**: [shapeworks::QOptimize::SetIterationCallback](../Classes/classshapeworks_1_1QOptimize.md#function-setiterationcallback)


### function RunProcrustes

```cpp
void RunProcrustes()
```

Run an iteration of procrustes. 

### function OptimizeStart

```cpp
void OptimizeStart()
```


### function OptimizerStop

```cpp
void OptimizerStop()
```


### function ReadTransformFile

```cpp
void ReadTransformFile()
```


### function ReadPrefixTransformFile

```cpp
void ReadPrefixTransformFile(
    const std::string & s
)
```


### function InitializeSampler

```cpp
void InitializeSampler()
```


### function GetMinNeighborhoodRadius

```cpp
double GetMinNeighborhoodRadius()
```


### function AddSinglePoint

```cpp
void AddSinglePoint()
```


### function Initialize

```cpp
void Initialize()
```


### function AddAdaptivity

```cpp
void AddAdaptivity()
```


### function RunOptimize

```cpp
void RunOptimize()
```


### function SetInitialCorrespondenceMode

```cpp
void SetInitialCorrespondenceMode()
```


### function IterateCallback

```cpp
virtual void IterateCallback(
    itk::Object * ,
    const itk::EventObject & 
)
```


**Reimplemented by**: [shapeworks::QOptimize::IterateCallback](../Classes/classshapeworks_1_1QOptimize.md#function-iteratecallback)


### function ComputeEnergyAfterIteration

```cpp
void ComputeEnergyAfterIteration()
```


### function SetCotanSigma

```cpp
void SetCotanSigma()
```


### function WriteTransformFile

```cpp
void WriteTransformFile(
    int iter =-1
) const
```


### function WriteTransformFile

```cpp
void WriteTransformFile(
    std::string iter_prefix
) const
```


### function WriteTransformFiles

```cpp
void WriteTransformFiles(
    int iter =-1
) const
```


### function WriteTransformFiles

```cpp
void WriteTransformFiles(
    std::string iter_prefix
) const
```


### function WritePointFiles

```cpp
void WritePointFiles(
    int iter =-1
)
```


### function WritePointFiles

```cpp
void WritePointFiles(
    std::string iter_prefix
)
```


### function WritePointFilesWithFeatures

```cpp
void WritePointFilesWithFeatures(
    int iter =-1
)
```


### function WritePointFilesWithFeatures

```cpp
void WritePointFilesWithFeatures(
    std::string iter_prefix
)
```


### function WriteEnergyFiles

```cpp
void WriteEnergyFiles()
```


### function WriteCuttingPlanePoints

```cpp
void WriteCuttingPlanePoints(
    int iter =-1
)
```


### function WriteParameters

```cpp
void WriteParameters(
    std::string output_dir =""
)
```


### function ReportBadParticles

```cpp
void ReportBadParticles()
```


### function SetParameters

```cpp
void SetParameters()
```


### function WriteModes

```cpp
void WriteModes()
```


### function PrintStartMessage

```cpp
void PrintStartMessage(
    std::string str,
    unsigned int vlevel =0
) const
```


### function PrintDoneMessage

```cpp
void PrintDoneMessage(
    unsigned int vlevel =0
) const
```


### function UpdateExportablePoints

```cpp
virtual void UpdateExportablePoints()
```


**Reimplemented by**: [shapeworks::QOptimize::UpdateExportablePoints](../Classes/classshapeworks_1_1QOptimize.md#function-updateexportablepoints)


### function GetCheckpointDir

```cpp
std::string GetCheckpointDir()
```


## Protected Attributes Documentation

### variable m_sampler

```cpp
std::shared_ptr< Sampler > m_sampler;
```


### variable m_procrustes

```cpp
itk::ParticleProcrustesRegistration< 3 >::Pointer m_procrustes;
```


### variable m_good_bad

```cpp
itk::ParticleGoodBadAssessment< float, 3 >::Pointer m_good_bad;
```


### variable m_verbosity_level

```cpp
unsigned int m_verbosity_level = 0;
```


### variable m_local_points

```cpp
std::vector< std::vector< itk::Point< double > > > m_local_points;
```


### variable m_global_points

```cpp
std::vector< std::vector< itk::Point< double > > > m_global_points;
```


### variable m_checkpoint_counter

```cpp
int m_checkpoint_counter = 0;
```


### variable m_procrustes_counter

```cpp
int m_procrustes_counter = 0;
```


### variable m_saturation_counter

```cpp
int m_saturation_counter = 0;
```


### variable m_disable_procrustes

```cpp
bool m_disable_procrustes = true;
```


### variable m_use_cutting_planes

```cpp
bool m_use_cutting_planes = false;
```


### variable m_optimizing

```cpp
bool m_optimizing = false;
```


### variable m_use_regression

```cpp
bool m_use_regression = false;
```


### variable m_use_mixed_effects

```cpp
bool m_use_mixed_effects = false;
```


### variable m_domains_per_shape

```cpp
unsigned int m_domains_per_shape = 1;
```


### variable m_domain_type

```cpp
shapeworks::DomainType m_domain_type = shapeworks::DomainType::Image;
```


### variable m_number_of_particles

```cpp
std::vector< int > m_number_of_particles;
```


### variable m_transform_file

```cpp
std::string m_transform_file;
```


### variable m_prefix_transform_file

```cpp
std::string m_prefix_transform_file;
```


### variable m_output_dir

```cpp
std::string m_output_dir;
```


### variable m_output_transform_file

```cpp
std::string m_output_transform_file;
```


### variable m_output_transform_files

```cpp
bool m_output_transform_files = false;
```


### variable m_mesh_based_attributes

```cpp
bool m_mesh_based_attributes = false;
```


### variable m_use_xyz

```cpp
std::vector< bool > m_use_xyz;
```


### variable m_use_normals

```cpp
std::vector< bool > m_use_normals;
```


### variable m_attributes_per_domain

```cpp
std::vector< int > m_attributes_per_domain;
```


### variable m_distribution_domain_id

```cpp
int m_distribution_domain_id = -1;
```


### variable m_output_cutting_plane_file

```cpp
std::string m_output_cutting_plane_file;
```


### variable m_processing_mode

```cpp
int m_processing_mode = 3;
```


### variable m_adaptivity_mode

```cpp
int m_adaptivity_mode = 0;
```


### variable m_adaptivity_strength

```cpp
double m_adaptivity_strength = 0.0;
```


### variable m_pairwise_potential_type

```cpp
int m_pairwise_potential_type = 0;
```


### variable m_timepts_per_subject

```cpp
unsigned int m_timepts_per_subject = 1;
```


### variable m_optimization_iterations

```cpp
int m_optimization_iterations = 2000;
```


### variable m_optimization_iterations_completed

```cpp
int m_optimization_iterations_completed = 0;
```


### variable m_iterations_per_split

```cpp
int m_iterations_per_split = 1000;
```


### variable m_initialization_criterion

```cpp
double m_initialization_criterion = 1e-6;
```


### variable m_optimization_criterion

```cpp
double m_optimization_criterion = 1e-6;
```


### variable m_use_shape_statistics_in_init

```cpp
bool m_use_shape_statistics_in_init = false;
```


### variable m_procrustes_interval

```cpp
unsigned int m_procrustes_interval = 3;
```


### variable m_procrustes_scaling

```cpp
int m_procrustes_scaling = 1;
```


### variable m_relative_weighting

```cpp
double m_relative_weighting = 1.0;
```


### variable m_initial_relative_weighting

```cpp
double m_initial_relative_weighting = 0.05;
```


### variable m_starting_regularization

```cpp
double m_starting_regularization = 1000;
```


### variable m_ending_regularization

```cpp
double m_ending_regularization = 1.0;
```


### variable m_recompute_regularization_interval

```cpp
int m_recompute_regularization_interval = 1;
```


### variable m_save_init_splits

```cpp
bool m_save_init_splits = false;
```


### variable m_checkpointing_interval

```cpp
unsigned int m_checkpointing_interval = 50;
```


### variable m_keep_checkpoints

```cpp
int m_keep_checkpoints = 0;
```


### variable m_cotan_sigma_factor

```cpp
double m_cotan_sigma_factor = 5.0;
```


### variable m_particle_flags

```cpp
std::vector< int > m_particle_flags;
```


### variable m_domain_flags

```cpp
std::vector< int > m_domain_flags;
```


### variable m_narrow_band

```cpp
double m_narrow_band = 0.0;
```


### variable m_narrow_band_set

```cpp
bool m_narrow_band_set = false;
```


### variable m_fixed_domains_present

```cpp
bool m_fixed_domains_present = false;
```


### variable m_use_shape_statistics_after

```cpp
int m_use_shape_statistics_after = -1;
```


### variable m_python_filename

```cpp
std::string m_python_filename;
```


### variable m_geodesics_enabled

```cpp
bool m_geodesics_enabled = false;
```


### variable m_geodesic_cache_size_multiplier

```cpp
size_t m_geodesic_cache_size_multiplier = 0;
```


### variable m_mode

```cpp
unsigned int m_mode = 0;
```


### variable m_spacing

```cpp
double m_spacing = 0;
```


### variable m_filenames

```cpp
std::vector< std::string > m_filenames;
```


### variable m_num_shapes

```cpp
int m_num_shapes = 0;
```


### variable m_energy_a

```cpp
std::vector< double > m_energy_a;
```


### variable m_energy_b

```cpp
std::vector< double > m_energy_b;
```


### variable m_total_energy

```cpp
std::vector< double > m_total_energy;
```


### variable m_log_energy

```cpp
bool m_log_energy = false;
```


### variable m_str_energy

```cpp
std::string m_str_energy;
```


### variable m_bad_ids

```cpp
std::vector< std::vector< int > > m_bad_ids;
```


### variable m_normal_angle

```cpp
double m_normal_angle = itk::Math::pi / 2.0;
```


### variable m_perform_good_bad

```cpp
bool m_perform_good_bad = false;
```


### variable m_cutting_planes_per_input

```cpp
std::vector< int > m_cutting_planes_per_input;
```


### variable m_spheres_per_input

```cpp
std::vector< int > m_spheres_per_input;
```


### variable m_file_output_enabled

```cpp
bool m_file_output_enabled = true;
```


### variable m_aborted

```cpp
bool m_aborted = false;
```


### variable m_cut_planes

```cpp
std::vector< std::array< itk::Point< double >, 3 > > m_cut_planes;
```


### variable m_total_iterations

```cpp
int m_total_iterations = 0;
```


### variable m_iteration_count

```cpp
int m_iteration_count = 0;
```


### variable m_split_number

```cpp
int m_split_number {0};
```


### variable m_rand

```cpp
std::mt19937 m_rand {42};
```


### variable m_iter_callback

```cpp
std::function< void(void)> m_iter_callback;
```


### variable show_visualizer

```cpp
bool show_visualizer = false;
```


### variable visualizer

```cpp
shapeworks::OptimizationVisualizer visualizer;
```


-------------------------------

Updated on 2021-11-19 at 20:38:58 +0000