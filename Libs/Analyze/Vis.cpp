
#include "Vis.h"


void Vis::visMesh(vtkSmartPointer<vtkPolyData> mesh, std::string window_title)
{
    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();

#if (VTK_MAJOR_VERSION < 6)
    mapper->SetInput(mesh);
#else
    mapper->SetInputData(mesh);
#endif
    mapper->SetScalarRange(mesh->GetScalarRange());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Vis::visMeshWithNormals(vtkSmartPointer<vtkPolyData> mesh, int ratio, std::string window_title)
{

    vtkSmartPointer<vtkPolyDataNormals> normals =
            vtkSmartPointer<vtkPolyDataNormals>::New(); 
#if (VTK_MAJOR_VERSION < 6)
    normals->SetInput(mesh);
#else
    normals->SetInputData(mesh);
#endif
    normals->ConsistencyOn();
    normals->AutoOrientNormalsOn();
    normals->ComputePointNormalsOn();
    normals->SplittingOff();
    normals->Update();


    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();

    mapper->SetInputConnection(normals->GetOutputPort());
    mapper->SetScalarRange(mesh->GetScalarRange());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();

    //    # We subsample the dataset because we want to glyph just a subset of
    //    # the points. Otherwise the display is cluttered and cannot be easily
    //    # read. The RandonModeOn and SetOnRatio combine to random select one out
    //    # of every 10 points in the dataset.
    vtkSmartPointer<vtkMaskPoints> ptMask = vtkSmartPointer<vtkMaskPoints>::New();
#if (VTK_MAJOR_VERSION < 6)
    ptMask->SetInput(mesh);
#else
    ptMask->SetInputData(mesh);
#endif
    ptMask->SetOnRatio(ratio);
    ptMask->RandomModeOn();

    //    # In this case we are using a cone as a glyph. We transform the cone so
    //    # its base is at 0,0,0. This is the point where glyph rotation occurs.
    vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
    cone->SetResolution(6);
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(0.5, 0.0, 0.0);

    vtkSmartPointer<vtkTransformPolyDataFilter> transformF = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformF->SetInputConnection(cone->GetOutputPort());
    transformF->SetTransform(transform);

    //    # vtkGlyph3D takes two inputs: the input point set (SetInput) which can be
    //    # any vtkDataSet; and the glyph (SetSource) which must be a vtkPolyData.
    //    # We are interested in orienting the glyphs by the surface normals that
    //    # we previosuly generated.
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetInputConnection(ptMask->GetOutputPort());
    glyph->SetSourceConnection(transformF->GetOutputPort());
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetScaleFactor(0.04);

    vtkSmartPointer<vtkPolyDataMapper> spikeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    spikeMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> spikeActor = vtkSmartPointer<vtkActor>::New();
    spikeActor->SetMapper(spikeMapper);
    spikeActor->GetProperty()->SetColor(0.0, 0.79, 0.34);
    spikeActor->GetProperty()->SetInterpolationToGouraud();

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->AddActor(spikeActor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Vis::visMeshWithParticlesNormals(vtkSmartPointer<vtkPolyData> mesh, vtkSmartPointer<vtkPolyData> particlesData, std::string window_title)
{
    vtkSmartPointer<vtkPolyDataNormals> normals =
            vtkSmartPointer<vtkPolyDataNormals>::New();
#if (VTK_MAJOR_VERSION < 6)
    normals->SetInput(particlesData);
#else
    normals->SetInputData(particlesData);
#endif
    normals->Update();

    // Create a mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();

    mapper->SetInputConnection(normals->GetOutputPort());
    mapper->SetScalarRange(mesh->GetScalarRange());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();

    //    # In this case we are using a cone as a glyph. We transform the cone so
    //    # its base is at 0,0,0. This is the point where glyph rotation occurs.
    vtkSmartPointer<vtkConeSource> cone = vtkSmartPointer<vtkConeSource>::New();
    cone->SetResolution(6);
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(0.5, 0.0, 0.0);

    vtkSmartPointer<vtkTransformPolyDataFilter> transformF = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformF->SetInputConnection(cone->GetOutputPort());
    transformF->SetTransform(transform);

    //    # vtkGlyph3D takes two inputs: the input point set (SetInput) which can be
    //    # any vtkDataSet; and the glyph (SetSource) which must be a vtkPolyData.
    //    # We are interested in orienting the glyphs by the surface normals that
    //    # we previosuly generated.
    vtkSmartPointer<vtkGlyph3D> glyph = vtkSmartPointer<vtkGlyph3D>::New();
    glyph->SetSourceConnection(transformF->GetOutputPort());
    glyph->SetVectorModeToUseNormal();
    glyph->SetScaleModeToScaleByVector();
    glyph->SetScaleFactor(4);

    vtkSmartPointer<vtkPolyDataMapper> spikeMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    spikeMapper->SetInputConnection(glyph->GetOutputPort());

    vtkSmartPointer<vtkActor> spikeActor = vtkSmartPointer<vtkActor>::New();
    spikeActor->SetMapper(spikeMapper);
    spikeActor->GetProperty()->SetColor(0.0, 0.79, 0.34);
    spikeActor->GetProperty()->SetInterpolationToGouraud();

    vtkSmartPointer<vtkPolyDataMapper> meshMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
#if (VTK_MAJOR_VERSION < 6)
    meshMapper->SetInput(mesh);
#else
    meshMapper->SetInputData(mesh);
#endif
    meshMapper->SetScalarRange(mesh->GetScalarRange());

    vtkSmartPointer<vtkActor> meshActor =
            vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->SetInterpolationToGouraud();
    meshActor->GetProperty()->SetColor(0,0,1);

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->AddActor(spikeActor);
    renderer->AddActor(meshActor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Vis::visMeshWithParticles(vtkSmartPointer<vtkPolyData> mesh,
                          vtkSmartPointer<vtkPoints> particles, float glyphRadius , std::string window_title)
{
    vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(particles);

    vtkSmartPointer<vtkPolyData> glyph =
            vtkSmartPointer<vtkPolyData>::New();

    // Create anything you want here, we will use a sphere for the demo.
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(glyphRadius);

    vtkSmartPointer<vtkGlyph3D> glyph3D =
            vtkSmartPointer<vtkGlyph3D>::New();
#if VTK_MAJOR_VERSION <= 5
    glyph3D->SetSource(sphereSource->GetOutput());
    glyph3D->SetInput(polydata);
#else
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
#endif
    glyph3D->Update();

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();
    actor->GetProperty()->SetColor(1,1,1);

    vtkSmartPointer<vtkPolyDataMapper> meshMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
#if (VTK_MAJOR_VERSION < 6)
    meshMapper->SetInput(mesh);
#else
    meshMapper->SetInputData(mesh);
#endif
    meshMapper->SetScalarRange(mesh->GetScalarRange());

    vtkSmartPointer<vtkActor> meshActor =
            vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->SetInterpolationToGouraud();
    meshActor->GetProperty()->SetColor(0,0,1);

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(meshActor);
    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Vis::visMeshWithParticles(vtkSmartPointer<vtkPolyData> mesh,
                          vtkSmartPointer<vtkPoints> particles, std::vector<double> glyphRadii , std::string window_title)
{
    int numberOfSpheres = particles->GetNumberOfPoints();

    // A renderer and render window
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer ( renderer );

    // An interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow ( renderWindow );

    vtkSmartPointer<vtkPolyDataMapper> meshMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
#if (VTK_MAJOR_VERSION < 6)
    meshMapper->SetInput(mesh);
#else
    meshMapper->SetInputData(mesh);
#endif
    meshMapper->SetScalarRange(mesh->GetScalarRange());

    vtkSmartPointer<vtkActor> meshActor =
            vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->SetInterpolationToGouraud();
    meshActor->GetProperty()->SetDiffuseColor(0.6,0.6,0.6);
    renderer->AddActor(meshActor);

    for (int i = 0; i < numberOfSpheres; ++i)
    {
        vtkSmartPointer<vtkSphereSource> source =
                vtkSmartPointer<vtkSphereSource>::New();
        double x, y, z;
        double p[3];

        particles->GetPoint(i,p);
        x = p[0]; y = p[1]; z = p[2];

        source->SetRadius(glyphRadii[i]);
        source->SetCenter(x, y, z);
        source->SetPhiResolution(11);
        source->SetThetaResolution(21);
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection ( source->GetOutputPort());
        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper ( mapper );
        actor->SetOrigin(x, y, z);

        double r, g, b;
        // r = 1; g = 1; b = 1;
        r = vtkMath::Random(.4, 1.0);
        g = vtkMath::Random(.4, 1.0);
        b = vtkMath::Random(.4, 1.0);
        actor->GetProperty()->SetDiffuseColor(r, g, b);
        actor->GetProperty()->SetDiffuse(.8);
        actor->GetProperty()->SetSpecular(.5);
        actor->GetProperty()->SetSpecularColor(1.0,1.0,1.0);
        actor->GetProperty()->SetSpecularPower(30.0);
        renderer->AddActor ( actor );

    }

    renderer->SetBackground ( .3, .4, .5 );

    // Render and interact
    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
}

void Vis::visMeshWithParticles(vtkSmartPointer<vtkPolyData> mesh,
                          vtkSmartPointer<vtkPoints> particles,
                          vtkSmartPointer<vtkPoints> particles2,
                          float glyphRadius, std::string window_title)
{
    vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(particles);

    vtkSmartPointer<vtkPolyData> polydata2 =
            vtkSmartPointer<vtkPolyData>::New();
    polydata2->SetPoints(particles2);

    // Create anything you want here, we will use a sphere for the demo.
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(glyphRadius);

    vtkSmartPointer<vtkGlyph3D> glyph3D =
            vtkSmartPointer<vtkGlyph3D>::New();
    vtkSmartPointer<vtkGlyph3D> glyph3D_2 =
            vtkSmartPointer<vtkGlyph3D>::New();
#if VTK_MAJOR_VERSION <= 5
    glyph3D->SetSource(sphereSource->GetOutput());
    glyph3D->SetInput(polydata);

    glyph3D_2->SetSource(sphereSource->GetOutput());
    glyph3D_2->SetInput(polydata2);
#else
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);

    glyph3D_2->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D_2->SetInputData(polydata2);
#endif
    glyph3D->Update();
    glyph3D_2->Update();

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> mapper2 =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputConnection(glyph3D_2->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();
    actor->GetProperty()->SetColor(1,0,0);

    vtkSmartPointer<vtkActor> actor2 =
            vtkSmartPointer<vtkActor>::New();
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetInterpolationToGouraud();
    actor2->GetProperty()->SetColor(0,1,0);


    vtkSmartPointer<vtkPolyDataMapper> meshMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
#if (VTK_MAJOR_VERSION < 6)
    meshMapper->SetInput(mesh);
#else
    meshMapper->SetInputData(mesh);
#endif
    meshMapper->SetScalarRange(mesh->GetScalarRange());

    vtkSmartPointer<vtkActor> meshActor =
            vtkSmartPointer<vtkActor>::New();
    meshActor->SetMapper(meshMapper);
    meshActor->GetProperty()->SetInterpolationToGouraud();
    meshActor->GetProperty()->SetColor(0,0,1);

    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->AddActor(meshActor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Vis::visParticles(vtkSmartPointer<vtkPoints> particles, float glyphRadius, std::string window_title)
{
    vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(particles);

    vtkSmartPointer<vtkPolyData> glyph =
            vtkSmartPointer<vtkPolyData>::New();

    // Create anything you want here, we will use a sphere for the demo.
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(glyphRadius);

    vtkSmartPointer<vtkGlyph3D> glyph3D =
            vtkSmartPointer<vtkGlyph3D>::New();
#if VTK_MAJOR_VERSION <= 5
    glyph3D->SetSource(sphereSource->GetOutput());
    glyph3D->SetInput(polydata);
#else
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
#endif
    glyph3D->Update();

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();


    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Vis::visParticles(vtkSmartPointer<vtkPolyData> polydata, float glyphRadius , std::string window_title)
{
    vtkSmartPointer<vtkPolyData> glyph =
            vtkSmartPointer<vtkPolyData>::New();

    // Create anything you want here, we will use a sphere for the demo.
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(glyphRadius);

    vtkSmartPointer<vtkGlyph3D> glyph3D =
            vtkSmartPointer<vtkGlyph3D>::New();
#if VTK_MAJOR_VERSION <= 5
    glyph3D->SetSource(sphereSource->GetOutput());
    glyph3D->SetInput(polydata);
#else
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
#endif
    glyph3D->Update();

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();


    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

void Vis::visParticles(vtkSmartPointer<vtkPoints> particles,
                  vtkSmartPointer<vtkPoints> particles2,
                  float glyphRadius, std::string window_title)
{
    vtkSmartPointer<vtkPolyData> polydata =
            vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(particles);

    vtkSmartPointer<vtkPolyData> polydata2 =
            vtkSmartPointer<vtkPolyData>::New();
    polydata2->SetPoints(particles2);

    // Create anything you want here, we will use a sphere for the demo.
    vtkSmartPointer<vtkSphereSource> sphereSource =
            vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(glyphRadius);

    vtkSmartPointer<vtkGlyph3D> glyph3D =
            vtkSmartPointer<vtkGlyph3D>::New();
    vtkSmartPointer<vtkGlyph3D> glyph3D_2 =
            vtkSmartPointer<vtkGlyph3D>::New();
#if VTK_MAJOR_VERSION <= 5
    glyph3D->SetSource(sphereSource->GetOutput());
    glyph3D->SetInput(polydata);

    glyph3D_2->SetSource(sphereSource->GetOutput());
    glyph3D_2->SetInput(polydata2);
#else
    glyph3D->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D->SetInputData(polydata);

    glyph3D_2->SetSourceConnection(sphereSource->GetOutputPort());
    glyph3D_2->SetInputData(polydata2);
#endif
    glyph3D->Update();
    glyph3D_2->Update();

    // Visualize
    vtkSmartPointer<vtkPolyDataMapper> mapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    vtkSmartPointer<vtkPolyDataMapper> mapper2 =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper2->SetInputConnection(glyph3D_2->GetOutputPort());

    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetInterpolationToGouraud();
    actor->GetProperty()->SetColor(1,0,0);

    vtkSmartPointer<vtkActor> actor2 =
            vtkSmartPointer<vtkActor>::New();
    actor2->SetMapper(mapper2);
    actor2->GetProperty()->SetInterpolationToGouraud();
    actor2->GetProperty()->SetColor(0,1,0);


    // Visualize
    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->AddActor(actor2);
    renderer->SetBackground(1,1,1); // Background color white

    renderWindow->SetSize(800,800); //(width, height)
    renderWindow->SetWindowName(window_title.c_str());
    renderWindow->Render();
    renderWindowInteractor->Start();
}

