#include <Visualization/Viewer.h>

#include <vtkRenderer.h>
#include <vtkImageActor.h>
//#include <vtkImageViewer2.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkProperty.h>

Viewer::Viewer()
{

  this->renderer_ = vtkSmartPointer<vtkRenderer>::New();
  this->count_ = 0;
  this->tile_layout_width_ = 4;
  this->tile_layout_height_ = 4;
  this->camera_ = this->renderer_->GetActiveCamera();
}

void Viewer::set_interactor( vtkRenderWindowInteractor* interactor )
{
  this->interactor_ = interactor;
}

void Viewer::add_input( vtkSmartPointer<vtkPolyData> polyData )
{
  std::cerr << polyData->GetNumberOfPoints() << "\n";

  if ( this->count_ >= this->renderers_.size() )
  {
    std::cerr << "aborting\n";
    return;
  }

  this->mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->actor_ = vtkSmartPointer<vtkActor>::New();

  this->mapper_->SetInputData( polyData );
  this->actor_->SetMapper( this->mapper_ );

  this->actor_->GetProperty()->SetDiffuseColor( 1, 191.0 / 255.0, 0 );

  //this->actor->GetProperty()->SetDiffuseColor( 1.0000, 0.3882, 0.2784 );
  //this->actor->GetProperty()->SetAmbientColor( 1.0000, 0.3882, 0.2784 );
  this->actor_->GetProperty()->SetSpecular( 0.2 );
  this->actor_->GetProperty()->SetSpecularPower( 15 );

  //this->actor->GetProperty()->SetDiffuse( 1 );
  //this->actor->GetProperty()->SetAmbient( 1 );
//  this->actor->GetProperty()->SetRepresentationToWireframe();
  this->mapper_->ScalarVisibilityOff();

  vtkSmartPointer<vtkRenderer> ren = this->renderers_[this->count_];

  ren->AddActor( this->actor_ );
  ren->ResetCamera();
  //double viewport[4] = {0.0, 0.0, 0.5, 1.0};

  this->count_++;
}

vtkRenderer* Viewer::get_renderer()
{
  return this->renderer_;
}

void Viewer::set_render_window( vtkRenderWindow* renderWindow )
{
  this->render_window_ = renderWindow;
  this->render_window_->AddRenderer( this->renderer_ );

  this->setup_renderers();
}

void Viewer::setup_renderers()
{
  for ( int i = 0; i < this->renderers_.size(); i++ )
  {
    this->render_window_->RemoveRenderer( this->renderers_[i] );
  }

  this->renderers_.clear();

  this->count_ = 0;

  int* size = this->render_window_->GetSize();
  std::cerr << "window size = " << size[0] << " x " << size[1] << "\n";

  int width = this->tile_layout_width_;
  int height = this->tile_layout_height_;
  int total = width * height;

  float margin = 0.005;

  float tile_height = ( 1.0f - ( margin * ( height + 1 ) ) ) / height;
  float tile_width = ( 1.0f - ( margin * ( width + 1 ) ) ) / width;

  std::cerr << "width_ratio = " << tile_width << "\n";
  std::cerr << "height_ratio = " << tile_height << "\n";

  float step_x = tile_width + margin;
  float step_y = tile_height + margin;

  for ( int y = 0; y < height; y++ )
  {
    for ( int x = 0; x < width; x++ )
    {
      int i = ( y * width ) + x;

      vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();
      ren->SetActiveCamera( this->camera_ );
      this->renderers_.push_back( ren );

      double viewport[4] = {0.0, 0.0, 0.0, 0.0};

      // horizontal
      viewport[0] = margin + ( x * step_x );
      viewport[2] = viewport[0] + tile_width;

      // vertical
      viewport[1] = margin + ( ( ( height - 1 ) - y ) * step_y );
      viewport[3] = viewport[1] + tile_height;

      std::cerr << "viewport = {" << viewport[0] << ", " << viewport[1] << ", " << viewport[2] << ", " << viewport[3] << "}\n";

      ren->SetViewport( viewport );
      //ren->SetBackground( .2, .2 * count, .1 * count );

      int mod = 3;
      int mod2 = 2;
      int mod3 = 1;

      if (width == 6)
      {
        mod = 4;
        mod2 = 3;
        mod3 = 2;
      }

      float color = 0.2 + ( 0.04 * (i % mod) );


      color = color - (0.02 * (i % mod2));

      color = color + (0.04 * (i % mod3));

      ren->SetBackground( color, color, color );

      this->render_window_->AddRenderer( ren );
      //ren->Render();
    }
  }

  this->render_window_->Render();
}

void Viewer::set_tile_layout( int width, int height )
{
  this->tile_layout_width_ = width;
  this->tile_layout_height_ = height;
}
