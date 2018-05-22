#include <iostream>
#include <queue>
#include <fstream>
#include <string>
#ifndef IGL_VIEWER_WITH_NANOGUI
int main()
{
  std::cerr<<
    "Error: recompile with LIBIGL_VIEWER_WITH_NANOGUI defined."<<std::endl;
  return EXIT_FAILURE;
}
#else
#include <nanogui/formhelper.h>
#include <nanogui/screen.h>
#include <nanogui/layout.h>
#include <nanogui/slider.h>
#include <nanogui/widget.h>
#include <nanogui/textbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/colorwheel.h>
#include <nanogui/popupbutton.h>

#include <igl/readOBJ.h>
#include <igl/remove_unreferenced.h>
#include <igl/slice.h>
#include <igl/readOBJ.h>
#include <igl/viewer/Viewer.h>
#include <Eigen/Sparse>
#include "itkvtkFunctions.h"
#include "computeFunctions.h"


using namespace std;
using namespace Eigen;

Eigen::MatrixXd TV;
Eigen::MatrixXi TT;
Eigen::MatrixXi TF;

Eigen::MatrixXd W;//(17352, 1024);
vector<Eigen::MatrixXd> Wvec;
Eigen::MatrixXd V;

Eigen::MatrixXd Vref;
Eigen::MatrixXd Vtemp;
Eigen::MatrixXi Fref;
Eigen::MatrixXd Vcontrol_static;
Eigen::MatrixXi Fcontrol_static;
Eigen::MatrixXd Vpca_mode;
Eigen::MatrixXi Fpca_mode;
Eigen::MatrixXd Vshape;
Eigen::MatrixXi Fshape;
Eigen::MatrixXd Vmean_space;
Eigen::MatrixXi Fmean_space;

bool pre_computed = false;
bool points_flag = true;
Eigen::RowVector3d mesh_diffuse_color(0.88f,0.64f,0.41f);
float sig = 0;
float step_size = 0.1;
int pca_mode_number = 0;


bool disp_shape = false;
// int N_total = 89;
int data_val = 0;



/*////////////////////////////////////////////////////////////////////////////////////////
MAIN ROUTINE
*/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[])
{ 

  using namespace igl;
  bool forward = true;
  
  igl::readOBJ(TUTORIAL_SHARED_PATH "/basemesh_new60.obj",Vref,Fref);
  Vref *= Eigen::AngleAxisd(-90*3.14/180,
          Eigen::Vector3d(-1,0,-0)).toRotationMatrix(); 
  // Tetrahedralize the interior
  // igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);
  
  TV = Vref;
  TF = Fref;
  TT = TF;

  // read the text file with paths to all the control points
  readOBJ(TUTORIAL_SHARED_PATH "/controlpoints_new60.obj", Vcontrol_static, Fcontrol_static);
  Vcontrol_static *= Eigen::AngleAxisd(-90*3.14/180,
          Eigen::Vector3d(-1,0,-0)).toRotationMatrix(); 
  readOBJ(TUTORIAL_SHARED_PATH "/meanshape_new60.obj", Vmean_space, Fmean_space);
  Vmean_space *= Eigen::AngleAxisd(-90*3.14/180,
          Eigen::Vector3d(-1,0,-0)).toRotationMatrix(); 

  // read the eigenvalues file and store it in a vector
  // generate a random color matrix
  Eigen::MatrixXd C(Vcontrol_static.rows(), Vcontrol_static.cols());
  for(int i = 0; i<Vcontrol_static.rows(); i++){

    float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    C(i,0) = r;
    C(i,1) = g;
    C(i,2) = b;
  }

  /*////////////////////////////////////////////////////////////////////////////////////////
  LOADING TEXT FILES
  */////////////////////////////////////////////////////////////////////////////////////////

  // read all the PCA configurations
  vector<string> v;
  string line;
  ifstream myfile(TUTORIAL_SHARED_PATH "/pcapaths_new60.txt");
  
  if(myfile.is_open()){
    while ( getline (myfile,line) )
      {
        v.push_back(line);
      }
  }

  myfile.close();

  // read all the eigenvalues
  vector<float> eigenvalue;
  ifstream myfile1(TUTORIAL_SHARED_PATH "/eigenvalues_new60.txt");

  if(myfile1.is_open()){
    while( getline (myfile1, line) ){
      eigenvalue.push_back(stof(line));
    }
  }
  myfile1.close();
  
  // read raw shapes
  vector<string> v_shapes;
  ifstream myfile2(TUTORIAL_SHARED_PATH "/shapepaths_new60.txt");
  
  if(myfile2.is_open()){
    while ( getline (myfile2,line) )
      {
        v_shapes.push_back(line);
      }
  }
  myfile2.close();
  // readOBJ(v_shapes[1], Vcontrol_static, Fcontrol_static);
  


  /*////////////////////////////////////////////////////////////////////////////////////////
  STARTING VIZ
  */////////////////////////////////////////////////////////////////////////////////////////
  
  Eigen::VectorXi b;
  Vtemp = W * (Vmean_space.rowwise() + RowVector3d(1,0,0));
  igl::viewer::Viewer viewer;
  viewer.data.set_mesh(TV, TF);
  viewer.data.set_vertices(Vtemp);
  viewer.data.set_colors(mesh_diffuse_color);
  viewer.core.invert_normals = true;
  V = Vmean_space;
  viewer.data.set_points(V,C);
    viewer.callback_key_pressed = 
      [&](igl::viewer::Viewer & viewer,unsigned int key,int mods)->bool
    {
      switch(key)
      {
        default: 
          return false;
      }
    };

  // set viewer call back init with all the nanogui functionality

  viewer.callback_init = [&](igl::viewer::Viewer& viewer)
  {
    
    nanogui::Window *window = new nanogui::Window(viewer.screen, "Control Panel");
        window->setPosition(Vector2i(10, 10));
        window->setLayout(new nanogui::GroupLayout());

    /*////////////////////////////////////////////////////////////////////////////////////////
    CHECKBOX FOR DISPLAYING POINTS
    */////////////////////////////////////////////////////////////////////////////////////////

    nanogui::CheckBox *cb_displaypoints = new nanogui::CheckBox(window, "Display points",
            [&](bool state) { 

              // assign the flag for displaying the points
              points_flag = state;
              if(!state){

                // clear the points and reset the display
                viewer.data.clear();
                viewer.data.set_mesh(TV, TF);
                viewer.data.set_vertices(Vtemp);
                viewer.data.set_colors(mesh_diffuse_color);
                // if(count_keyframes){viewer.data.set_vertices(Vtemp);}
                
              }
              else{
                  if(disp_shape){
                    viewer.data.set_points(Vshape, C); 
                  }
                  else{
                    viewer.data.set_points(V, C);
                  }

              }

            });

    cb_displaypoints->setChecked(true);

    /*////////////////////////////////////////////////////////////////////////////////////////
    CHECKBOXES FOR ANIMATION AND DISPLAY SHAPES FLAG
    */////////////////////////////////////////////////////////////////////////////////////////

    // Add a checkbox for the animation control
    nanogui::CheckBox *cb_animate = new nanogui::CheckBox(window, "Animate",
            [&](bool state) { viewer.core.is_animating = state; }
        );
    cb_animate->setChecked(false);

    // Checkbox for displaying the direct shapes
    nanogui::CheckBox *cb_displayshapes = new nanogui::CheckBox(window, "Display shapes",
            [&](bool state) { 
              if(viewer.core.is_animating){
                cout << "Stop the Animation to activate individual shape display"<< endl;
              }
              disp_shape = state;
              }
        );
    cb_displayshapes->setChecked(false);

    /*////////////////////////////////////////////////////////////////////////////////////////
    SIGMA VARIATION SLIDER
    */////////////////////////////////////////////////////////////////////////////////////////

    // Setup a panel
    nanogui::Widget *panel_sigmaslider = new nanogui::Widget(window);
        panel_sigmaslider->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
                                       nanogui::Alignment::Middle, 0, 20));

    // textbox and the slider
    new nanogui::Label(panel_sigmaslider, "sigma :", "sans-bold");
    nanogui::Slider *slider_sigma = new nanogui::Slider(panel_sigmaslider);
    
    slider_sigma->setValue(0.5f);
    slider_sigma->setFixedWidth(100);

    nanogui::TextBox *textBox_sigma = new nanogui::TextBox(panel_sigmaslider);
    textBox_sigma->setFixedSize(Vector2i(80, 25));
    textBox_sigma->setValue("0");

    slider_sigma->setCallback([&,textBox_sigma](float value) {
        if(!viewer.core.is_animating){

          double y = 10 * (4 * value - 2);
          int rounded = (int)(y + 0.5);
          sig = (float) rounded / 10.0;

          readOBJ(v[pca_mode_number], Vpca_mode, Fpca_mode);
          Vpca_mode *= Eigen::AngleAxisd(-90*3.14/180,
          Eigen::Vector3d(-1,0,-0)).toRotationMatrix(); 
          V = mode_variation(Vpca_mode, Vmean_space, eigenvalue[pca_mode_number], sig);
          Vtemp = W * (V.rowwise() + RowVector3d(1,0,0));
          viewer.data.set_vertices(Vtemp);

          // display the overlay control points if the flag is true
          if(points_flag){
            viewer.data.set_points(V, C);
          }

          viewer.data.compute_normals(); 
          textBox_sigma->setValue(std::to_string((double) rounded / 10.0)); 
          
        }

    });


    /*////////////////////////////////////////////////////////////////////////////////////////
    ANIMATION FRAMES PER SECONDS TEXTBOX
    */////////////////////////////////////////////////////////////////////////////////////////

    // For setting the animation fps
    nanogui::Widget *panel2 = new nanogui::Widget(window);
    panel2->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle, 0, 20));

    // textbox for the FPS
    {
    new nanogui::Label(panel2, "Animation FPS :", "sans-bold");
    auto intBox = new nanogui::IntBox<int>(panel2);
    intBox->setEditable(true);
    intBox->setFixedSize(Vector2i(60, 25));
    intBox->setFontSize(16);
    // intBox->setFormat("[1-30][0-30]*");
    intBox->setValue(30);
    intBox->setCallback([&,intBox](int value){
      viewer.core.animation_max_fps = value;
      intBox->setValue(value);
    });
    }

    /*////////////////////////////////////////////////////////////////////////////////////////
    CONTROL POINTS SIZE TEXTBOX
    */////////////////////////////////////////////////////////////////////////////////////////

    // Setup a panel
    nanogui::Widget *panel3 = new nanogui::Widget(window);
    panel3->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle, 0, 20));

    // textbox for the point size 
    {
    new nanogui::Label(panel3, "Point Size :", "sans-bold");
    auto intBox = new nanogui::IntBox<int>(panel3);
    intBox->setEditable(true);
    intBox->setFixedSize(Vector2i(60, 25));
    intBox->setFontSize(16);
    intBox->setValue(30);
    // intBox->setFormat("[1-30][0-30]*");
    intBox->setCallback([&,intBox](int value){
      viewer.core.point_size = value;
      intBox->setValue(value);
    });
    }

    /*////////////////////////////////////////////////////////////////////////////////////////
    MESH COLOR PICKER
    */////////////////////////////////////////////////////////////////////////////////////////

    // Setup the panel
    nanogui::Widget *panel4 = new nanogui::Widget(window);
    panel4->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle, 0, 20));

    // color picker for the mesh
    {
    new nanogui::Label(panel4, "Mesh Diffuse Color :", "sans-bold");
        nanogui::PopupButton *popupBtn = new nanogui::PopupButton(panel4, "", 0);
        popupBtn->setBackgroundColor(nanogui::Color(255, 228, 58, 255));
        popupBtn->setFontSize(16);
        popupBtn->setFixedSize(Vector2i(60, 25));
        nanogui::Popup *popup = popupBtn->popup();
        popup->setLayout(new nanogui::GroupLayout());

        nanogui::ColorWheel *colorwheel = new nanogui::ColorWheel(popup);
        colorwheel->setColor(popupBtn->backgroundColor());

        nanogui::Button *colorBtn = new nanogui::Button(popup, "Pick");
        colorBtn->setFixedSize(Vector2i(100, 25));
        nanogui::Color c = colorwheel->color();
        colorBtn->setBackgroundColor(c);

        colorwheel->setCallback([&,colorBtn](const nanogui::Color &value) {
            colorBtn->setBackgroundColor(value);
            mesh_diffuse_color[0] = value[0];
            mesh_diffuse_color[1] = value[1];
            mesh_diffuse_color[2] = value[2];
            viewer.data.set_colors(mesh_diffuse_color);
        });

        colorBtn->setChangeCallback([colorBtn, popupBtn](bool pushed) {
            if (pushed) {
                popupBtn->setBackgroundColor(colorBtn->backgroundColor());
                popupBtn->setPushed(false);
            }
        });
      }

    /*////////////////////////////////////////////////////////////////////////////////////////
    BACKGROUND COLOR PICKER
    */////////////////////////////////////////////////////////////////////////////////////////

      // Setup the panel
    nanogui::Widget *panel41 = new nanogui::Widget(window);
    panel41->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle, 0, 20));

    // color picker for the background
    {
    new nanogui::Label(panel41, "Background Color :", "sans-bold");
        nanogui::PopupButton *popupBtn = new nanogui::PopupButton(panel41, "", 0);
        popupBtn->setBackgroundColor(nanogui::Color(1.0f, 1.0f, 1.0f, 1.0f));
        popupBtn->setFontSize(16);
        popupBtn->setFixedSize(Vector2i(60, 25));
        nanogui::Popup *popup = popupBtn->popup();
        popup->setLayout(new nanogui::GroupLayout());

        nanogui::ColorWheel *colorwheel = new nanogui::ColorWheel(popup);
        colorwheel->setColor(popupBtn->backgroundColor());

        nanogui::Button *colorBtn = new nanogui::Button(popup, "Pick");
        colorBtn->setFixedSize(Vector2i(100, 25));
        nanogui::Color c = colorwheel->color();
        colorBtn->setBackgroundColor(c);

        colorwheel->setCallback([&,colorBtn](const nanogui::Color &value) {
            colorBtn->setBackgroundColor(value);
            Eigen::Vector4f background_color;
            background_color << value[0], value[1], value[2], 1.0f;
            viewer.core.background_color = background_color;

        });

        colorBtn->setChangeCallback([colorBtn, popupBtn](bool pushed) {
            if (pushed) {
                popupBtn->setBackgroundColor(colorBtn->backgroundColor());
                popupBtn->setPushed(false);
            }
        });
      }

    /*////////////////////////////////////////////////////////////////////////////////////////
    PCA MODE OF VARIATION TEXTBOX
    */////////////////////////////////////////////////////////////////////////////////////////

    // Setup the panel
    nanogui::Widget *panel5 = new nanogui::Widget(window);
    panel5->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle, 0, 20));

    // Textbox for the PCA Mode
    {
    new nanogui::Label(panel5, "PCA Mode :", "sans-bold");
    auto intBox = new nanogui::IntBox<int>(panel5);
    intBox->setEditable(true);
    intBox->setFixedSize(Vector2i(60, 25));
    intBox->setFontSize(16);
    intBox->setValue(0);
    // intBox->setFormat("[1-30][0-30]*");
    intBox->setCallback([&,intBox](int value){
      pca_mode_number = value;
      
      if(value > v.size() - 1) pca_mode_number = v.size() - 1;
      intBox->setValue(value);
    });
    }

    /*////////////////////////////////////////////////////////////////////////////////////////
    SHAPE DISPLAY SELECTOR TEXTBOX
    */////////////////////////////////////////////////////////////////////////////////////////

    // Setup the panel
    nanogui::Widget *panel6 = new nanogui::Widget(window);
    panel6->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal,
        nanogui::Alignment::Middle, 0, 20));

    // Textbox for the Shape Display
    {
    new nanogui::Label(panel6, "Display Shapes :", "sans-bold");
    auto intBox = new nanogui::IntBox<int>(panel6);
    intBox->setEditable(true);
    intBox->setFixedSize(Vector2i(60, 25));
    intBox->setFontSize(16);
    intBox->setValue(0);
    // intBox->setFormat("[1-30][0-30]*");

    intBox->setCallback([&,intBox](int value){
      if(disp_shape && value < v_shapes.size()){

        viewer.data.clear();
        readOBJ(v_shapes[value], Vshape, Fshape);
        // find the point interpolation
  
        Vtemp = W * (Vshape.rowwise() + RowVector3d(1,0,0));
        viewer.data.set_mesh(TV, TF);
        viewer.data.set_vertices(Vtemp);
        if(points_flag){
              viewer.data.set_points(Vshape, C);
        }
        viewer.data.set_colors(mesh_diffuse_color);
        viewer.data.compute_normals();
        intBox->setValue(value);
      }
      // if(value == 1){
      //   viewer.data.clear();
      //   readOBJ(v_shapes[value], Vshape, Fshape);
      //   viewer.data.set_mesh(Vref, Fref);
      //   if(points_flag){
      //         viewer.data.set_points(Vshape, C);
      //   }
      //   viewer.data.set_colors(mesh_diffuse_color);
      //   viewer.data.compute_normals();
      //   intBox->setValue(value);
      // }
      
    });
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////
    TOGGLE BETWEEN DATA
    */////////////////////////////////////////////////////////////////////////////////////////
    // {
      new nanogui::Label(window, "Dataset", "sans-bold");
      nanogui::ComboBox *selector = new nanogui::ComboBox(window, { "30 young heads", "Normative-MRI", "CT-Metopic", 
        "JM 1024 Normal", "JM 1024 Metopic", "JM W/O Size 1024 Normal", "JM W/O Size 1024 Metopic", "Hands", "New Joint Model"});
      selector->setSelectedIndex(0);

      selector->setCallback([&,selector](int value){
          if(value == data_val){
              cout << "It's the same" << endl;
          }
          else{
            ///////////////////////// For the 30 shapes ///////////////////////////
            if(value == 0){
              cout << "30 clean head shapes" << endl;
              igl::readOBJ(TUTORIAL_SHARED_PATH "/tissue_cut33.obj",Vref,Fref);

              // Tetrahedralize the interior
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);
              // read the text file with paths to all the control points
              readOBJ(TUTORIAL_SHARED_PATH "/pts.06_500.obj", Vcontrol_static, Fcontrol_static);
              readOBJ(TUTORIAL_SHARED_PATH "/meanshape.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/pcamodepaths.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  {
                    v.push_back(line);
                  }
              }
              myfile.close();

              // read all the eigenvalues
              ifstream myfile1(TUTORIAL_SHARED_PATH "/eigenvalues.txt");
              eigenvalue.clear();
              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              myfile1.close();
              
              // read raw shapes
              ifstream myfile2(TUTORIAL_SHARED_PATH "/shapepaths.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            ///////////////////////// For the CT-Metopic heads ///////////////////////////
            else if(value == 2){
              cout << "The CT Metopic heads" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/ctmetopic_basemesh.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/ctmetopic_cp.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/ctmetopic_meanshape.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/ctmetopic_pcamodepaths.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/ctmetopic_eigenvalues.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/ctmetopic_shapepaths.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            else if(value == 3){
              cout << "Normative from Joint Model with 1024 points" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/jm_normative_basemesh.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/jm_normative_cp_1024.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/jm_normative_meanshape_1024.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/jm_normative_pcamodepaths_1024.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/jm_normative_eigenvalues_1024.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/jm_normative_shapepaths_1024.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            else if(value == 5){
              cout << "Normative without size from Joint Model with 1024 points" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/jm_normative_basemesh_wos.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/jm_normative_cp_wos1024.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/jm_normative_meanshape_wos1024.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/jm_normative_pcamodepaths_wos1024.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/jm_normative_eigenvalues_wos1024.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/jm_normative_shapepaths_wos1024.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            else if(value == 4){
              cout << "CT Metopic from Joint Model with 1024 points" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/jm_pathological_basemesh_2.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/jm_pathological_cp_1024.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/jm_pathological_meanshape_1024.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/jm_pathological_pcamodepaths_1024.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/jm_pathological_eigenvalues_1024.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/jm_pathological_shapepaths_1024.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            else if(value == 6){
              cout << "CT Metopic without size from Joint Model with 1024 points" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/jm_pathological_basemesh_wos.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/jm_pathological_cp_wos1024.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/jm_pathological_meanshape_wos1024.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/jm_pathological_pcamodepaths_wos1024.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/jm_pathological_eigenvalues_wos1024.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/jm_pathological_shapepaths_wos1024.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            else if(value == 7){
              cout << "Hands Dataset with 1024 points" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/hands_basemesh.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/hands_controlpoints.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/hands_meanshape.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/hands_pcapaths.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/hands_eigenvalues.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/hands_shapepaths.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            else if(value == 8){
              cout << "New Joint Model" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/new_jm_basemesh.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/new_jm_controlpoints.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/new_jm_meanshape.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/new_jm_pcapaths.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/new_jm_eigenvalues.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/new_jm_shapepaths.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }
            
            
            
            ///////////////////////// For the 1year 100 Normal MRI scans ///////////////////////////
            else{
              cout << "The 100 normative MRI skin shapes" << endl;

              igl::readOBJ(TUTORIAL_SHARED_PATH "/normativemri_basemesh.obj",Vref,Fref);
  
              igl::copyleft::tetgen::tetrahedralize(Vref,Fref,"pYa500", TV,TT,TF);

              readOBJ(TUTORIAL_SHARED_PATH "/normativemri_cp.obj", Vcontrol_static, Fcontrol_static);
              
              readOBJ(TUTORIAL_SHARED_PATH "/normativemri_mean.obj", Vmean_space, Fmean_space);

              ifstream myfile(TUTORIAL_SHARED_PATH "/normativemri_pcamodepaths.txt");
              v.clear();
              if(myfile.is_open()){
                while ( getline (myfile,line) )
                  { 
                    v.push_back(line);
                  }
              }
              // cout << v.size() << endl;
              myfile.close();

              // read all the eigenvalues
              eigenvalue.clear();
              ifstream myfile1(TUTORIAL_SHARED_PATH "/normativemri_eigenvalues.txt");

              if(myfile1.is_open()){
                while( getline (myfile1, line) ){
                  eigenvalue.push_back(stof(line));
                }
              }
              
              myfile1.close();

              
              ifstream myfile2(TUTORIAL_SHARED_PATH "/normativemri_shapepaths.txt");
              v_shapes.clear();
              if(myfile2.is_open()){
                while ( getline (myfile2,line) )
                  {
                    v_shapes.push_back(line);
                  }
              }
              myfile2.close();
            }

            // we will always have to recompute the the W matrix
            Wvec = W_precomputation(Vcontrol_static, TV, TT, TF);
            W = Wvec[0];
            Vtemp = W * (Vmean_space.rowwise() + RowVector3d(1,0,0));
            viewer.data.clear();
            viewer.data.set_mesh(TV, TF);
            viewer.data.set_vertices(Vtemp);
            viewer.data.set_colors(mesh_diffuse_color);
            V = Vmean_space;
            if(points_flag){viewer.data.set_points(V,C);}
            data_val = value;
          }
      });

    // }
    /*////////////////////////////////////////////////////////////////////////////////////////
    MAIN ANIMATION ROUTINE
    */////////////////////////////////////////////////////////////////////////////////////////

    // The Main Animation Routine
      viewer.callback_pre_draw = [&, textBox_sigma, slider_sigma](igl::viewer::Viewer & viewer)->bool
      {
      glDisable(GL_CULL_FACE);

      if(viewer.core.is_animating)
      {
          
            readOBJ(v[pca_mode_number], Vpca_mode, Fpca_mode);
            Vpca_mode *= Eigen::AngleAxisd(-90*3.14/180,
          Eigen::Vector3d(-1,0,-0)).toRotationMatrix(); 
            // find the point interpolation
            V = mode_variation(Vpca_mode, Vmean_space, eigenvalue[pca_mode_number], sig);
            Vtemp = W * (V.rowwise() + RowVector3d(1,0,0));
            viewer.data.set_vertices(Vtemp);

            // display the overlay control points if the flag is true
            if(points_flag){
              viewer.data.set_points(V, C);
            }

            viewer.data.compute_normals();
            viewer.data.set_colors(mesh_diffuse_color);

            if(forward){sig = sig + step_size;}
            else{sig = sig - step_size;}

            if(sig >= 2){forward = false;}
            if(sig <= -2){forward = true;}
            textBox_sigma->setValue(std::to_string(sig)); 
            slider_sigma->setValue((sig + 2)/4.0);

      }
      return false;
    };

    // Generate menu
    viewer.screen->performLayout();

    return false;
  };

  // final touches before launching the viewer
  viewer.core.show_lines = false;
  viewer.data.set_face_based(true);
  viewer.core.rotation_type = 
    igl::viewer::ViewerCore::ROTATION_TYPE_TWO_AXIS_VALUATOR_FIXED_UP;
  viewer.launch();
}
#endif