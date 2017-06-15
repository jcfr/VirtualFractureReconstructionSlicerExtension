#ifndef __RigidRegistrator_txx
#define __RigidRegistrator_txx

#include "RigidRegistrator.h"

template< class TImage >
RigidRegistrator< TImage >
::RigidRegistrator()
:ImageRegistrationMethod<TImage, TImage>()
  {
  this->SetTransform(TransformType::New());
  this->GetTypedTransform()->SetIdentity();
  this->SetInitialTransformParameters(
                  this->GetTypedTransform()->GetParameters());

  this->SetInterpolator(InterpolatorType::New());

  m_OptimizerMethod = ONEPLUSONEPLUSGRADIENT;
  m_OptimizerNumberOfIterations = 350 ;
  m_OptimizerScales.set_size(6) ;
  m_OptimizerScales[0] = 150; // rotations
  m_OptimizerScales[1] = 150;
  m_OptimizerScales[2] = 150;
  m_OptimizerScales[3] = 1;  // offset
  m_OptimizerScales[4] = 1;
  m_OptimizerScales[5] = 1;

  this->SetMetric(MetricType::New());
  m_MetricNumberOfSpatialSamples = 100000 ;

  m_SecondaryOptimizer = 0;

  m_Observer = 0;
  }

template< class TImage >
RigidRegistrator< TImage >
::~RigidRegistrator()
  {
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetOptimizerToOnePlusOne()
  {
  m_OptimizerMethod = ONEPLUSONE;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetOptimizerToGradient()
  {
  m_OptimizerMethod = GRADIENT;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::SetOptimizerToOnePlusOnePlusGradient()
  {
  m_OptimizerMethod = ONEPLUSONEPLUSGRADIENT;
  }

template< class TImage >
void
RigidRegistrator< TImage >
::Initialize() throw (ExceptionObject)
  {
  this->GetInterpolator()->SetInputImage(this->GetMovingImage()) ;
  if (m_MetricNumberOfSpatialSamples == 0)
  {
    this->GetTypedMetric()->UseAllPixelsOn();
    std::cout<<"Using all pixels as samples"<<std::endl;
  }
  else
  {
    this->GetTypedMetric()->UseAllPixelsOff();
    this->GetTypedMetric()->SetNumberOfSpatialSamples(
        m_MetricNumberOfSpatialSamples );
    std::cout<<"Using "<<m_MetricNumberOfSpatialSamples<<" samples"<<std::endl;
  }

  switch(m_OptimizerMethod)
    {
    case ONEPLUSONE:
      {
      OnePlusOneOptimizerType::Pointer opt = OnePlusOneOptimizerType::New();
      opt->SetNormalVariateGenerator( OptimizerNormalGeneratorType::New() );
      opt->SetMaximumIteration( m_OptimizerNumberOfIterations);
      opt->SetEpsilon(1e-10);
      opt->Initialize(1.01); // Initial search radius
      opt->SetScales( m_OptimizerScales );
      this->SetOptimizer(opt);
      this->SetSecondaryOptimizer(0);
      if(m_Observer)
        {
        opt->AddObserver(itk::IterationEvent(), m_Observer);
        }
      break;
      }
    case GRADIENT:
      {
      GradientOptimizerType::Pointer opt = GradientOptimizerType::New();
      opt->SetMaximize(false);
      opt->SetStepLength(5);
      opt->SetStepTolerance(1e-10);
      opt->SetMaximumIteration(m_OptimizerNumberOfIterations);
      opt->SetScales( m_OptimizerScales );
      this->SetOptimizer(opt);
      this->SetSecondaryOptimizer(0);
      if(m_Observer)
        {
        opt->AddObserver(itk::IterationEvent(), m_Observer);
        }
      break;
      }
    case ONEPLUSONEPLUSGRADIENT:
      {
      OnePlusOneOptimizerType::Pointer initOpt = OnePlusOneOptimizerType::New();
      initOpt->SetNormalVariateGenerator( OptimizerNormalGeneratorType::New() );
      initOpt->SetMaximumIteration( m_OptimizerNumberOfIterations);
      initOpt->SetEpsilon(1e-10);
      initOpt->Initialize(1.01); // Initial search radius
      initOpt->SetScales( m_OptimizerScales );

      GradientOptimizerType::Pointer opt = GradientOptimizerType::New();
      opt->SetMaximize(false);
      opt->SetStepLength(0.25);
      opt->SetStepTolerance(1e-10);
      opt->SetMaximumIteration(25);
      opt->SetScales( m_OptimizerScales );

      this->SetOptimizer(initOpt);
      this->SetSecondaryOptimizer(opt);
      if(m_Observer)
        {
        initOpt->AddObserver(itk::IterationEvent(), m_Observer);
        opt->AddObserver(itk::IterationEvent(), m_Observer);
        }

      break;
      }
    }

  try
    {
    Superclass::Initialize();
    }
  catch(ExceptionObject e)
    {
    throw(e);
    }
  }

template< class TImage >
void
RigidRegistrator< TImage >
::Update()
  {
  try
    {
    Superclass::Update();
    }
  catch(ExceptionObject &e)
    {
    this->PrintError(e) ;
    }
  catch(...)
    {
    this->PrintUncaughtError() ;
    }

  if(m_SecondaryOptimizer)
    {
    m_SecondaryOptimizer->SetCostFunction(this->GetMetric());
    m_SecondaryOptimizer->SetInitialPosition(this->GetLastTransformParameters());
    try
      {
      m_SecondaryOptimizer->StartOptimization();
      }
    catch(ExceptionObject &e)
      {
      this->SetLastTransformParameters(
                     m_SecondaryOptimizer->GetCurrentPosition() );
      this->GetTypedTransform()->SetParameters(
                     this->GetLastTransformParameters() );

      this->PrintError(e) ;
      }

    this->SetLastTransformParameters(
                   m_SecondaryOptimizer->GetCurrentPosition() );
    this->GetTypedTransform()->SetParameters(
                   this->GetLastTransformParameters() );
    }

  }

template< class TImage >
void
RigidRegistrator< TImage >
::PrintUncaughtError()
{
  std::cout << "-------------------------------------------------"
            << std::endl;
  std::cout << "Exception caught in RigidRegistrator:" << std::endl;
  std::cout << "unknown exception caught !!!" << std::endl;
  std::cout << "-------------------------------------------------"
            << std::endl;
}

template< class TImage >
void
RigidRegistrator< TImage >
::PrintError(ExceptionObject &e)
{
  std::cout << "-------------------------------------------------"
            << std::endl;
  std::cout << "Exception caught in RigidRegistrator:" << std::endl;
  std::cout << e << std::endl;
  std::cout << "-------------------------------------------------"
            << std::endl;
}

#endif //__RigidRegistrator_txx
