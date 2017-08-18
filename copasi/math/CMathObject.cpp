// Copyright (C) 2017 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and University of
// of Connecticut School of Medicine.
// All rights reserved.

// Copyright (C) 2011 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#include <cmath>

#include "copasi.h"

#include "CMathObject.h"
#include "CMathExpression.h"
#include "CMathContainer.h"

#include "model/CMetab.h"
#include "model/CCompartment.h"
#include "model/CModel.h"
#include "function/CExpression.h"
#include "utilities/utility.h"
// static
C_FLOAT64 CMathObject::InvalidValue = std::numeric_limits< C_FLOAT64 >::quiet_NaN();

// static
void CMathObject::initialize(CMathObject * pObject,
                             C_FLOAT64 * pValue,
                             const CMath::ValueType & valueType,
                             const CMath::EntityType & entityType,
                             const CMath::SimulationType & simulationType,
                             const bool & isIntensiveProperty,
                             const bool & isInitialValue,
                             const CDataObject * pDataObject)
{
  pObject->mpValue = pValue;
  pObject->mValueType = valueType;
  pObject->mEntityType = entityType;
  pObject->mSimulationType = simulationType;
  pObject->mIsIntensiveProperty = isIntensiveProperty;
  pObject->mIsInitialValue = isInitialValue;
  pObject->mpDataObject = pDataObject;

  pdelete(pObject->mpExpression);
  pObject->mpCorrespondingProperty = NULL;
}

CMathObject::CMathObject():
  CObjectInterface(),
  mpExpression(NULL),
  mpValue(&InvalidValue),
  mPrerequisites(),
  mValueType(CMath::ValueType::Undefined),
  mEntityType(CMath::EntityType::Undefined),
  mSimulationType(CMath::SimulationType::Undefined),
  mIsIntensiveProperty(false),
  mIsInitialValue(false),
  mpCorrespondingProperty(NULL),
  mpDataObject(NULL)
{}

CMathObject::CMathObject(const CMathObject & src):
  CObjectInterface(src),
  mpExpression(src.mpExpression),
  mpValue(src.mpValue),
  mPrerequisites(src.mPrerequisites),
  mValueType(src.mValueType),
  mEntityType(src.mEntityType),
  mSimulationType(src.mSimulationType),
  mIsIntensiveProperty(src.mIsIntensiveProperty),
  mIsInitialValue(src.mIsInitialValue),
  mpCorrespondingProperty(src.mpCorrespondingProperty),
  mpDataObject(src.mpDataObject)
{}

// virtual
CMathObject::~CMathObject()
{
  pdelete(mpExpression);
}

void CMathObject::copy(const CMathObject & src, CMathContainer & container)
{
  assert(&src != this);
  *this = src;

  if (src.mpExpression != NULL)
    {
      mpExpression = CMathExpression::copy(*src.mpExpression, container);
    }
  else
    {
      mpExpression = NULL;
    }
}

void CMathObject::relocate(const CMathContainer * pContainer,
                           const std::vector< CMath::sRelocate > & relocations)
{
  pContainer->relocateValue(mpValue, relocations);
  pContainer->relocateObject(mpCorrespondingProperty, relocations);

  if (mpExpression != NULL)
    {
      mpExpression->relocate(pContainer, relocations);
    }

  pContainer->relocateObjectSet(mPrerequisites, relocations);
}

void CMathObject::moved()
{
  mpExpression = NULL;
}

// virtual
CCommonName CMathObject::getCN() const
{
  if (mpDataObject == NULL)
    return CCommonName("");

  return mpDataObject->getCN();
}

std::string CMathObject::getObjectDisplayName() const
{
  if (mpDataObject == NULL)
    {
      return "Math Container Internal Object";
    }

  return mpDataObject->getObjectDisplayName();
}

//virtual
const CObjectInterface * CMathObject::getObject(const CCommonName & cn) const
{
  if (mpDataObject == NULL)
    return NULL;

  const CObjectInterface * pObject = mpDataObject->getObject(cn);

  if (pObject != mpDataObject)
    return pObject;

  return this;
}

// virtual
const CObjectInterface::ObjectSet & CMathObject::getPrerequisites() const
{
  return mPrerequisites;
}

// virtual
bool CMathObject::isPrerequisiteForContext(const CObjectInterface * pObject,
    const CCore::SimulationContextFlag & context,
    const CObjectInterface::ObjectSet & changedObjects) const
{
  // This method should only be called for objects which are prerequisites.
  // We check for this only in debug mode.
  assert(mPrerequisites.find(pObject) != mPrerequisites.end());

  switch (mEntityType)
    {
      case CMath::EntityType::Moiety:

        if (context.isSet(CCore::SimulationContext::UpdateMoieties) &&
            mValueType == CMath::ValueType::TotalMass)
          {
            return true;
          }

        if (context.isSet(CCore::SimulationContext::UseMoieties) &&
            mValueType == CMath::ValueType::DependentMass)
          {
            return true;
          }

        return false;
        break;

      case CMath::EntityType::Species:

        // For species we need to account for the duality of the intensive and extensive value
        if (mValueType != CMath::ValueType::Value)
          return true;

        if (context.isSet(CCore::SimulationContext::UseMoieties) &&
            mSimulationType == CMath::SimulationType::Dependent &&
            !mIsIntensiveProperty)
          {
            if (mpCorrespondingProperty != pObject)
              {
                return true;
              }

            return false;
          }

        // If the value is in the context, it does not depend on the object.
        if (changedObjects.find(this) != changedObjects.end())
          return false;

        if (mIsIntensiveProperty)
          {
            // Densities which are not in the context have to be recalculated.
            return true;
          }
        else
          {
            // Amount which are determine by assignment need to be recalculated.
            if (mSimulationType == CMath::SimulationType::Assignment)
              return true;

            // If the concentration was changed in the context we need to recalculate.
            if (changedObjects.find(mpCorrespondingProperty) != changedObjects.end())
              return true;

            // If the concentration is calculated by an assignment we need to recalculate.
            if (mpCorrespondingProperty->getSimulationType() == CMath::SimulationType::Assignment)
              return true;

            return false;
          }

        break;

      case CMath::EntityType::Event:

        if (context.isSet(CCore::SimulationContext::EventHandling) &&
            mValueType == CMath::ValueType::Discontinuous)
          {
            switch (mpExpression->getRoot()->mainType() | mpExpression->getRoot()->subType())
              {
                case (CEvaluationNode::MainType::CHOICE | CEvaluationNode::SubType::IF):
                {
                  const CMathObject * pMathObject = dynamic_cast< const CMathObject * >(pObject);

                  if (pMathObject != NULL &&
                      pMathObject->mValueType == CMath::ValueType::EventTrigger)
                    {
                      return false;
                    }

                  return true;
                }
                break;

                case (CEvaluationNode::MainType::FUNCTION | CEvaluationNode::SubType::FLOOR):
                  return false;
                  break;

                case (CEvaluationNode::MainType::FUNCTION | CEvaluationNode::SubType::CEIL):
                  return false;
                  break;

                default:
                  return true;
              }
          }

        return true;
        break;

      case CMath::EntityType::Delay:

        if (context.isSet(CCore::SimulationContext::EventHandling))
          {
            return true;
          }

        return (mValueType == CMath::ValueType::DelayLag);

        break;

      default:
        return true;
    }

  // This should never be reached.
  return true;
}

// virtual
void CMathObject::print(std::ostream * ostream) const
{
  (*ostream) << *mpValue;
}

void CMathObject::setDataObject(const CDataObject * pDataObject)
{
  mpDataObject = pDataObject;
}

// virtual
const CDataObject * CMathObject::getDataObject() const
{
  return mpDataObject;
}

// virtual
void * CMathObject::getValuePointer() const
{
  return mpValue;
}

// virtual
void CMathObject::calculateValue()
{
  // This method should only be called if there is something to calculate, i.e.
  // mpExpression != NULL
  assert(mpExpression != NULL);

  *mpValue = mpExpression->value();

#ifdef COPASI_DEBUG_TRACE

  // Check for NaN
  if (isnan(*mpValue) && mpExpression->getInfix() != "")
    {
      std::cout << "NaN Value for: " << getCN() << std::endl;
    }

#endif // COPASI_DEBUG_TRACE

  // For an extensive transient value of a dependent species we have 2
  // possible assignments depending on the context.
  //   1) Conversion from the intensive property
  //   2) Dependent mass off a moiety
  //
  // The solution is that the moiety automatically updates the value in conjunction
  // with the dependency graph omitting the value in the update sequence if the context
  // is CMath::UseMoieties.
}

const CMath::ValueType & CMathObject::getValueType() const
{
  return mValueType;
}

void CMathObject::setValueType(const CMath::ValueType & valueType)
{
  mValueType = valueType;
}

const CMath::EntityType & CMathObject::getEntityType() const
{
  return mEntityType;
}

const CMath::SimulationType & CMathObject::getSimulationType() const
{
  return mSimulationType;
}

void CMathObject::setSimulationType(const CMath::SimulationType & simulationType)
{
  mSimulationType = simulationType;
}

const bool & CMathObject::isIntensiveProperty() const
{
  return mIsIntensiveProperty;
}

const bool & CMathObject::isInitialValue() const
{
  return mIsInitialValue;
}

const CMathObject * CMathObject::getCorrespondingProperty() const
{
  return mpCorrespondingProperty;
}

bool CMathObject::setExpression(const std::string & infix,
                                const bool & isBoolean,
                                CMathContainer & container)
{
  bool success = true;
  CExpression Expression;
  Expression.setIsBoolean(isBoolean);

  success &= Expression.setInfix(infix);
  CObjectInterface::ContainerList ListOfContainer;
  ListOfContainer.push_back(&container);
  success &= Expression.compile(ListOfContainer);
  success &= setExpression(Expression, container);

  return success;
}

bool CMathObject::setExpression(const CExpression & expression,
                                CMathContainer & container)
{
  bool success = true;

  success &= createConvertedExpression(&expression, container);

  return success;
}

bool CMathObject::setExpressionPtr(CMathExpression * pMathExpression)
{
  bool success = true;

  mPrerequisites.clear();

  pdelete(mpExpression);
  mpExpression = pMathExpression;

  if (mpExpression != NULL)
    {
      success &= mpExpression->compile();
      compileExpression();
    }
  else
    {
      success = false;
    }

  return success;
}

const CMathExpression * CMathObject::getExpressionPtr() const
{
  return mpExpression;
}

void CMathObject::appendDelays(CMath::DelayData & Delays) const
{
  if (mpExpression == NULL)
    {
      return;
    }

  std::vector< CEvaluationNode * >::const_iterator it = mpExpression->getNodeList().begin();
  std::vector< CEvaluationNode * >::const_iterator end = mpExpression->getNodeList().end();

  for (; it != end; ++it)
    {
      switch ((*it)->mainType())
        {
          case CEvaluationNode::MainType::DELAY:
          {
            CEvaluationNode * pValueExpression = static_cast< CEvaluationNode * >((*it)->getChild());
            std::string Expression = static_cast< CEvaluationNode * >(pValueExpression->getSibling())->buildInfix();

            CMath::DelayData::iterator found = Delays.find(Expression);

            if (found == Delays.end())
              {
                found = Delays.insert(std::make_pair(Expression, CMath::DelayValueData()));
              }

            Expression = pValueExpression->buildInfix();
            found->second.insert(std::make_pair(Expression, std::make_pair((*it)->buildInfix(), const_cast< CMathObject * >(this))));
          }
          break;

          default:
            break;
        }
    }

  return;
}

bool CMathObject::compile(CMathContainer & container)
{
  mPrerequisites.clear();
  bool success = true;

  switch (mValueType)
    {
      case CMath::ValueType::Undefined:
        success = false;
        break;

      case CMath::ValueType::Value:

        if (mIsInitialValue)
          {
            success = compileInitialValue(container);
          }
        else
          {
            success = compileValue(container);
          }

        break;

      case CMath::ValueType::Rate:
        success = compileRate(container);
        break;

      case CMath::ValueType::ParticleFlux:
        success = compileParticleFlux(container);
        break;

      case CMath::ValueType::Flux:
        success = compileFlux(container);
        break;

      case CMath::ValueType::Noise:

        if (mEntityType == CMath::EntityType::Reaction)
          {
            success = compileReactionNoise(container);
          }
        else
          {
            success = compileNoise(container);
          }

        break;

      case CMath::ValueType::ParticleNoise:
        success = compileReactionParticleNoise(container);
        break;

      case CMath::ValueType::Propensity:
        success = compilePropensity(container);
        break;

      case CMath::ValueType::TotalMass:
        success = compileTotalMass(container);
        break;

      case CMath::ValueType::DependentMass:
        success = compileDependentMass(container);
        break;

      case CMath::ValueType::TransitionTime:
        success = compileTransitionTime(container);
        break;

      case CMath::ValueType::Discontinuous:
        compileExpression();
        break;

      case CMath::ValueType::EventDelay:
      case CMath::ValueType::EventPriority:
      case CMath::ValueType::EventAssignment:
      case CMath::ValueType::EventTrigger:
      case CMath::ValueType::EventRoot:
      case CMath::ValueType::EventRootState:
        // These objects are compiled through the event compile,
        // which is executed after the object compile. It is therefore
        // correct to leave the object in its default state.
        break;

      case CMath::ValueType::__SIZE:
        break;
    }

  return success;
}

bool CMathObject::compileInitialValue(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  // Initial values are taken from the data model
  if (mpDataObject != NULL)
    {
      *mpValue = * (C_FLOAT64 *) mpDataObject->getValuePointer();
    }

  const CModelEntity * pEntity = dynamic_cast< const CModelEntity * >(mpDataObject->getObjectParent());
  const CMetab * pSpecies = NULL;

  // Only species have corresponding properties (extensive vs intensive).
  if (mEntityType == CMath::EntityType::Species)
    {
      pSpecies = static_cast< const CMetab * >(pEntity);

      if (mIsIntensiveProperty)
        {
          mpCorrespondingProperty = container.getMathObject(pSpecies->getInitialValueReference());
        }
      else
        {
          mpCorrespondingProperty = container.getMathObject(pSpecies->getInitialConcentrationReference());
        }
    }

  if (mIsIntensiveProperty)
    {
      switch (mSimulationType)
        {
          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Fixed:
          case CMath::SimulationType::ODE:
          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Dependent:
          case CMath::SimulationType::Conversion:
            success &= createIntensiveValueExpression(pSpecies, container);
            break;

          case CMath::SimulationType::Assignment:
            // Extensive Property * Conversion / Compartment Size
            success &= createConvertedExpression(pSpecies->getInitialExpressionPtr(), container);

            break;

          case CMath::SimulationType::Time:
          case CMath::SimulationType::Undefined:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }
  else
    {
      switch (mSimulationType)
        {
          case CMath::SimulationType::Fixed:
            break;

          case CMath::SimulationType::Assignment:
            if (pEntity != NULL)
              {
                success &= createConvertedExpression(pEntity->getInitialExpressionPtr(), container);
              }
            else
              {
                compileExpression();
              }

            break;

          case CMath::SimulationType::Conversion:
          {
            success &= createExtensiveValueExpression(pSpecies, container);
          }
          break;

          case CMath::SimulationType::Undefined:
          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Time:
          case CMath::SimulationType::ODE:
          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Dependent:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }

  return success;
}

bool CMathObject::compileValue(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CModelEntity * pEntity = NULL;
  const CMetab * pSpecies = NULL;

  if (mpDataObject != NULL)
    {
      pEntity = dynamic_cast< const CModelEntity * >(mpDataObject->getObjectParent());
    }

  // Only species have corresponding properties (extensive vs intensive).
  if (mEntityType == CMath::EntityType::Species)
    {
      pSpecies = static_cast< const CMetab * >(pEntity);

      if (mIsIntensiveProperty)
        {
          mpCorrespondingProperty = container.getMathObject(pSpecies->getValueReference());
        }
      else
        {
          mpCorrespondingProperty = container.getMathObject(pSpecies->getConcentrationReference());
        }
    }

  if (mIsIntensiveProperty)
    {
      switch (mSimulationType)
        {
          case CMath::SimulationType::Assignment:
            success &= createConvertedExpression(pSpecies->getExpressionPtr(), container);
            break;

          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Conversion:
            success &= createIntensiveValueExpression(pSpecies, container);
            break;

          case CMath::SimulationType::Undefined:
          case CMath::SimulationType::Fixed:
          case CMath::SimulationType::Time:
          case CMath::SimulationType::ODE:
          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Dependent:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }
  else
    {
      // Species need an additional conversion since the event targets the
      // intensive property.
      if (mEntityType == CMath::EntityType::Species)
        {
          success &= createExtensiveValueExpression(pSpecies, container);
        }

      switch (mSimulationType)
        {
          case CMath::SimulationType::Fixed:
          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Time:
          case CMath::SimulationType::ODE:
          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Conversion:
            break;

          case CMath::SimulationType::Dependent:
          {
            // We need to add the dependent number of the moiety as a possible
            // prerequisite.
            const CMoiety * pMoiety = pSpecies->getMoiety();
            const CMathObject * pDependentNumber =
              container.getMathObject(pMoiety->getDependentNumberReference());

            mPrerequisites.insert(pDependentNumber);
          }
          break;

          case CMath::SimulationType::Assignment:
            if (pEntity != NULL)
              {
                success &= createConvertedExpression(pEntity->getExpressionPtr(), container);
              }
            else
              {
                compileExpression();
              }

            break;

          case CMath::SimulationType::Undefined:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }

  return success;
}

bool CMathObject::compileRate(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CModelEntity * pEntity = NULL;
  const CMetab * pSpecies = NULL;

  if (mpDataObject != NULL)
    {
      pEntity = dynamic_cast< const CModelEntity * >(mpDataObject->getObjectParent());
    }

  // Only species have corresponding properties (extensive vs intensive).
  if (mEntityType == CMath::EntityType::Species)
    {
      pSpecies = static_cast< const CMetab * >(pEntity);
    }

  if (mIsIntensiveProperty)
    {
      // Only species have intensive properties.
      switch (mSimulationType)
        {
          case CMath::SimulationType::Assignment:
            success &= createIntensiveRateExpression(pSpecies, container);
            break;

          case CMath::SimulationType::Undefined:
          case CMath::SimulationType::Fixed:
          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Time:
          case CMath::SimulationType::ODE:
          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Dependent:
          case CMath::SimulationType::Conversion:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }
  else
    {

      switch (mSimulationType)
        {
          case CMath::SimulationType::Fixed:
            *mpValue = 0;
            break;

          case CMath::SimulationType::Time:
            *mpValue = 1;
            break;

          case CMath::SimulationType::ODE:

            if (mEntityType == CMath::EntityType::Species)
              {
                success &= createExtensiveODERateExpression(pSpecies, container);
              }
            else if (pEntity != NULL)
              {
                success &= createConvertedExpression(pEntity->getExpressionPtr(), container);
              }
            else
              {
                compileExpression();
              }

            break;

          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Dependent:
          {
            success &= createExtensiveReactionRateExpression(pSpecies, container);
          }
          break;

          case CMath::SimulationType::Assignment:
            // TODO When we have symbolic differentiation we can deal with this.
            break;

          case CMath::SimulationType::Undefined:
          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Conversion:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }

  return success;
}

bool CMathObject::compileParticleFlux(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CReaction * pReaction = static_cast< const CReaction * >(mpDataObject->getObjectParent());

  // We need to check whether this reaction is a single compartment reaction and scale
  // it if true.
  //   mParticleFlux = *mUnitScalingFactor * mFlux;
  //   mUnitScalingFactor = & pModel->getQuantity2NumberFactor();

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  Infix << pointerToString(&container.getQuantity2NumberFactor());
  Infix << "*";
  Infix << pointerToString(container.getMathObject(pReaction->getFluxReference())->getValuePointer());

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("ParticleFluxExpression", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::compileFlux(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CReaction * pReaction = static_cast< const CReaction * >(mpDataObject->getObjectParent());

  // We need to check whether this reaction is a single compartment reaction and scale it if true.
  //   mFlux = *mScalingFactor * mpFunction->calcValue(mMap.getPointers());
  //   mScalingFactor = compartment volume or 1

  pdelete(mpExpression);
  mpExpression = new CMathExpression(*pReaction->getFunction(),
                                     pReaction->getCallParameters(),
                                     container,
                                     !mIsInitialValue);

  if (pReaction->getScalingCompartment() != NULL &&
      pReaction->getEffectiveKineticLawUnitType() == CReaction::KineticLawUnit::ConcentrationPerTime)
    {
      CExpression Tmp(mpExpression->getObjectName(), &container);

      std::string Infix = pointerToString(container.getMathObject(pReaction->getScalingCompartment()->getValueReference())->getValuePointer()) + "*(" + mpExpression->getInfix() + ")";
      success &= Tmp.setInfix(Infix);
      success &= Tmp.compile();

      pdelete(mpExpression);
      mpExpression = new CMathExpression(Tmp, container, false);
    }

  compileExpression();

  return success;
}

bool CMathObject::compileNoise(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  // Reset the prerequisites
  mPrerequisites.clear();

  const CModelEntity * pEntity = NULL;
  const CMetab * pSpecies = NULL;

  if (mpDataObject != NULL)
    {
      pEntity = dynamic_cast< const CModelEntity * >(mpDataObject->getObjectParent());
    }

  // Only species have corresponding properties (extensive vs intensive).
  if (mEntityType == CMath::EntityType::Species)
    {
      pSpecies = static_cast< const CMetab * >(pEntity);
    }

  if (mIsIntensiveProperty)
    {
      // Only species have intensive properties.
      switch (mSimulationType)
        {
          case CMath::SimulationType::Assignment:
            success &= createIntensiveNoiseExpression(pSpecies, container);
            break;

          case CMath::SimulationType::Undefined:
          case CMath::SimulationType::Fixed:
          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Time:
          case CMath::SimulationType::ODE:
          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Dependent:
          case CMath::SimulationType::Conversion:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }
  else
    {
      switch (mSimulationType)
        {
          case CMath::SimulationType::ODE:

            if (pEntity != NULL &&
                pEntity->hasNoise())
              {
                if (mEntityType == CMath::EntityType::Species)
                  {
                    success &= createExtensiveNoiseExpression(pSpecies, container);
                  }
                else if (pEntity->getNoiseExpressionPtr() != NULL)
                  {
                    success &= createConvertedExpression(pEntity->getNoiseExpressionPtr(), container);
                  }

                container.hasNoiseInputObject(this);
                compileExpression();
              }

            else
              {
                *mpValue = 0.0;
              }

            break;

          case CMath::SimulationType::Independent:
          case CMath::SimulationType::Dependent:
            // TODO CRITICAL Implement me!
          {
            success &= createExtensiveReactionNoiseExpression(pSpecies, container);
          }

          break;

          case CMath::SimulationType::Fixed:
          case CMath::SimulationType::Time:
          case CMath::SimulationType::Assignment:
          case CMath::SimulationType::Undefined:
          case CMath::SimulationType::EventTarget:
          case CMath::SimulationType::Conversion:
            success = false;
            break;

          case CMath::SimulationType::__SIZE:
            break;
        }
    }

  return success;
}

bool CMathObject::compileReactionParticleNoise(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  // Reset the prerequisites
  mPrerequisites.clear();
  pdelete(mpExpression);

  const CReaction * pReaction = static_cast< const CReaction * >(mpDataObject->getObjectParent());

  // We need to check whether this reaction is a single compartment reaction and scale
  // it if true.
  //   mParticleNoise = *mUnitScalingFactor * mNoise;
  //   mUnitScalingFactor = & pModel->getQuantity2NumberFactor();

  if (pReaction->hasNoise())
    {
      std::string Infix = pointerToString(&container.getQuantity2NumberFactor());
      Infix += "*" + pointerToString(container.getMathObject(pReaction->getNoiseReference())->getValuePointer());

      mpExpression = new CMathExpression("ReactionParticleNoiseExpression", container);
      success &= mpExpression->setInfix(Infix);
      success &= mpExpression->compile();
    }

  compileExpression();

  return success;
}

bool CMathObject::compileReactionNoise(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  // Reset the prerequisites
  mPrerequisites.clear();
  pdelete(mpExpression);

  const CReaction * pReaction = static_cast< const CReaction * >(mpDataObject->getObjectParent());

  if (pReaction->hasNoise())
    {
      // We need to check whether this reaction is a single compartment reaction and scale it if true.
      //   mFlux = *mScalingFactor * mpFunction->calcValue(mMap.getPointers());
      //   mScalingFactor = compartment volume or 1

      mpExpression = new CMathExpression(*pReaction->getNoiseExpressionPtr(),
                                         container,
                                         !mIsInitialValue);

      if (pReaction->getScalingCompartment() != NULL &&
          pReaction->getEffectiveKineticLawUnitType() == CReaction::KineticLawUnit::ConcentrationPerTime)
        {
          CExpression Tmp(mpExpression->getObjectName(), &container);

          std::string Infix = pointerToString(container.getMathObject(pReaction->getScalingCompartment()->getValueReference())->getValuePointer());
          Infix += "*(" + mpExpression->getInfix() + ")";

          success &= Tmp.setInfix(Infix);
          success &= Tmp.compile();

          pdelete(mpExpression);
          mpExpression = new CMathExpression(Tmp, container, false);
        }

      container.hasNoiseInputObject(this);
    }

  compileExpression();

  return success;
}

bool CMathObject::compilePropensity(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CReaction * pReaction = static_cast< const CReaction * >(mpDataObject->getObjectParent());

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  // Propensity for reversible reactions must be NaN
  if (pReaction->isReversible())
    {
      Infix << "NAN";
    }
  else
    {
      // Propensity is the same as the flux, but it must not be negative.
      Infix << "max(0," << pointerToString(container.getMathObject(pReaction->getParticleFluxReference())->getValuePointer());

      // Apply correction for deterministic models
      if (container.getModel().getModelType() == CModel::deterministic)
        {
          std::ostringstream Divisor;
          Divisor.imbue(std::locale::classic());
          Divisor.precision(std::numeric_limits<double>::digits10 + 2);

          const CDataVector<CChemEqElement> & Substrates = pReaction->getChemEq().getSubstrates();
          CDataVector< CChemEqElement >::const_iterator itSubstrate = Substrates.begin();
          CDataVector< CChemEqElement >::const_iterator endSubstrate = Substrates.end();
          bool first = true;

          for (; itSubstrate != endSubstrate; ++itSubstrate)
            {
              const CMetab * pMetab = itSubstrate->getMetabolite();
              const std::string NumberPointer = pointerToString((pMetab != NULL) ? container.getMathObject(pMetab->getValueReference())->getValuePointer() : &CMathObject::InvalidValue);

              C_FLOAT64 Multiplicity = itSubstrate->getMultiplicity();

              Multiplicity -= 1.0; // Nothing to correct if the multiplicity is 1.

              if (Multiplicity > 2.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                {
                  if (!first)
                    {
                      Divisor << "*";
                    }

                  first = false;
                  Divisor << NumberPointer << "^" << Multiplicity;
                }
              else if (Multiplicity > 1.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                {
                  if (!first)
                    {
                      Divisor << "*";
                    }

                  first = false;
                  Divisor << NumberPointer;
                }

              while (Multiplicity > 1.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                {
                  Infix << "*(" << NumberPointer << "-" << Multiplicity << ")";
                  Multiplicity -= 1.0;
                }
            }

          if (Divisor.str() != "")
            {
              Infix << "/(" << Divisor.str() << ")";
            }
        }

      Infix << ")";
    }

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("PropensityExpression", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::compileTotalMass(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CMoiety * pMoiety = static_cast< const CMoiety *>(mpDataObject->getObjectParent());

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  std::vector< std::pair< C_FLOAT64, CMetab * > >::const_iterator it = pMoiety->getEquation().begin();
  std::vector< std::pair< C_FLOAT64, CMetab * > >::const_iterator end = pMoiety->getEquation().end();
  bool First = true;

  for (; it != end; ++it)
    {
      const C_FLOAT64 & Multiplicity = it->first;

      if (First || Multiplicity < 0.0)
        {
          Infix << Multiplicity;
        }
      else
        {
          Infix << "+" << Multiplicity;
        }

      First = false;

      Infix << "*";
      Infix << pointerToString(container.getMathObject(it->second->getValueReference())->getValuePointer());
    }

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("TotalMass", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::compileDependentMass(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CMoiety * pMoiety = static_cast< const CMoiety *>(mpDataObject->getObjectParent());

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  Infix << pointerToString(container.getMathObject(pMoiety->getTotalNumberReference())->getValuePointer());

  std::vector< std::pair< C_FLOAT64, CMetab * > >::const_iterator it = pMoiety->getEquation().begin();
  std::vector< std::pair< C_FLOAT64, CMetab * > >::const_iterator end = pMoiety->getEquation().end();
  bool First = true;

  // The first element in the equation is always the dependent species. We can directly update
  // its value and therefore point mpValue to it.
  mpValue = (C_FLOAT64 *) container.getMathObject(it->second->getValueReference())->getValuePointer();

  ++it;

  for (; it != end; ++it)
    {
      const C_FLOAT64 & Multiplicity = it->first;

      if (First || Multiplicity >= 0.0)
        {
          Infix << "-" << Multiplicity;
        }
      else
        {
          Infix << "+" << fabs(Multiplicity);
        }

      First = false;

      Infix << "*";
      Infix << pointerToString(container.getMathObject(it->second->getValueReference())->getValuePointer());
    }

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("DependentMass", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::compileTransitionTime(CMathContainer & container)
{
  bool success = true;

  // The default value is NaN
  *mpValue = InvalidValue;

  const CMetab * pSpecies = static_cast< const CMetab *>(mpDataObject->getObjectParent());

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  switch (pSpecies->getStatus())
    {
      case CModelEntity::Status::ODE:
        // mTT = *mpValue / fabs(mRate);
        Infix << "abs(";
        Infix << pointerToString(container.getMathObject(pSpecies->getValueReference())->getValuePointer());
        Infix << "/";
        Infix << pointerToString(container.getMathObject(pSpecies->getRateReference())->getValuePointer());
        Infix << ")";
        break;

      case CModelEntity::Status::REACTIONS:
      {
        std::ostringstream PositiveFlux;
        PositiveFlux.imbue(std::locale::classic());
        PositiveFlux.precision(std::numeric_limits<double>::digits10 + 2);

        std::ostringstream NegativeFlux;
        NegativeFlux.imbue(std::locale::classic());
        NegativeFlux.precision(std::numeric_limits<double>::digits10 + 2);

        std::string Key = pSpecies->getKey();
        bool First = true;

        CDataVectorN< CReaction >::const_iterator it = container.getModel().getReactions().begin();
        CDataVectorN< CReaction >::const_iterator end = container.getModel().getReactions().end();

        for (; it != end; ++it)
          {
            const CDataVector< CChemEqElement > &Balances =
              it->getChemEq().getBalances();
            CDataVector< CChemEqElement >::const_iterator itChem = Balances.begin();
            CDataVector< CChemEqElement >::const_iterator endChem = Balances.end();

            for (; itChem != endChem; ++itChem)
              if (itChem->getMetaboliteKey() == Key)
                break;

            if (itChem != endChem)
              {
                const C_FLOAT64 & Multiplicity = itChem->getMultiplicity();

                if (!First)
                  {
                    PositiveFlux << "+";
                    NegativeFlux << "+";
                  }

                PositiveFlux << "max(";
                NegativeFlux << "min(";

                if (Multiplicity == std::numeric_limits< C_FLOAT64 >::infinity())
                  {
                    PositiveFlux << "infinity*";
                    NegativeFlux << "infinity*";
                  }
                else if (Multiplicity == -std::numeric_limits< C_FLOAT64 >::infinity())
                  {
                    PositiveFlux << "-infinity*";
                    NegativeFlux << "-infinity*";
                  }
                // Remove multiplying with -1.0
                else if (-1.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() < Multiplicity &&
                         Multiplicity < -1.0 + 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                  {
                    PositiveFlux << "-";
                    NegativeFlux << "-";
                  }
                // Remove multiplying with 1.0
                else if (1.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() < Multiplicity &&
                         Multiplicity < 1.0 + 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                  {
                    // PositiveFlux << "+";
                    // NegativeFlux << "+";
                  }
                else
                  {
                    PositiveFlux << Multiplicity << "*";
                    NegativeFlux << Multiplicity << "*";
                  }

                PositiveFlux << pointerToString(container.getMathObject(it->getParticleFluxReference())->getValuePointer());
                NegativeFlux << pointerToString(container.getMathObject(it->getParticleFluxReference())->getValuePointer());

                PositiveFlux << ",0)";
                NegativeFlux << ",0)";

                First = false;
              }
          }

        if (!First)
          {
            Infix << "abs(";
            Infix << pointerToString(container.getMathObject(pSpecies->getValueReference())->getValuePointer());
            Infix << ")/if(";
            Infix << pointerToString(container.getMathObject(pSpecies->getRateReference())->getValuePointer());
            Infix << "<0,-(" << NegativeFlux.str() << ")," << PositiveFlux.str() << ")";
          }
      }
      break;

      default:
        break;
    }

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("TransitionTimeExpression", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

void CMathObject::compileExpression()
{
  // Reset the prerequisites
  mPrerequisites.clear();

  if (mpExpression == NULL) return;

  if (mIsInitialValue)
    {
      mpExpression->convertToInitialExpression();
    }

  mPrerequisites.insert(mpExpression->getPrerequisites().begin(),
                        mpExpression->getPrerequisites().end());

  if (mPrerequisites.empty())
    {
      calculateValue();
    }
}

bool CMathObject::createConvertedExpression(const CExpression * pExpression,
    CMathContainer & container)
{
  assert(pExpression != NULL);

  bool success = true;

  bool ReplaceDiscontinousNodes =
    !mIsInitialValue &&
    mValueType != CMath::ValueType::Discontinuous &&
    mValueType != CMath::ValueType::EventAssignment &&
    mValueType != CMath::ValueType::EventPriority &&
    mValueType != CMath::ValueType::EventDelay;

  pdelete(mpExpression);
  mpExpression = new CMathExpression(*pExpression, container, ReplaceDiscontinousNodes);
  compileExpression();

  return success;
}

bool CMathObject::createIntensiveValueExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  bool success = true;

  // mConc = *mpValue / (mpModel->getQuantity2NumberFactor() * mpCompartment->getValue());
  CObjectInterface * pNumber = NULL;
  CObjectInterface * pCompartment = NULL;

  if (mIsInitialValue)
    {
      pNumber = pSpecies->getInitialValueReference();
      pCompartment = pSpecies->getCompartment()->getInitialValueReference();
    }
  else
    {
      pNumber = pSpecies->getValueReference();
      pCompartment = pSpecies->getCompartment()->getValueReference();
    }

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  Infix << pointerToString(container.getMathObject(pNumber)->getValuePointer());
  Infix << "/(";
  Infix << pointerToString(&container.getQuantity2NumberFactor());
  Infix << "*";
  Infix << pointerToString(container.getMathObject(pCompartment)->getValuePointer());
  Infix << ")";

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("IntensiveValueExpression", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::createExtensiveValueExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  bool success = true;

  // mConc * mpCompartment->getValue() * mpModel->getQuantity2NumberFactor();

  CObjectInterface * pIntensiveValue = NULL;
  CObjectInterface * pCompartment = NULL;

  if (mIsInitialValue)
    {
      pIntensiveValue = pSpecies->getInitialConcentrationReference();
      pCompartment = pSpecies->getCompartment()->getInitialValueReference();
    }
  else
    {
      pIntensiveValue = pSpecies->getConcentrationReference();
      pCompartment = pSpecies->getCompartment()->getValueReference();
    }

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  Infix << pointerToString(&container.getQuantity2NumberFactor());
  Infix << "*";
  Infix << pointerToString(container.getMathObject(pIntensiveValue)->getValuePointer());
  Infix << "*";
  Infix << pointerToString(container.getMathObject(pCompartment)->getValuePointer());

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("ExtensiveValueExpression", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::createIntensiveRateExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  bool success = true;

  /*
    mConcRate =
      (mRate * mpModel->getQuantity2NumberFactor() - mConc * mpCompartment->getRate())
      / mpCompartment->getValue();
   */

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  Infix << "(";
  Infix << pointerToString(container.getMathObject(pSpecies->getRateReference())->getValuePointer());
  Infix << "/";
  Infix << pointerToString(&container.getQuantity2NumberFactor());

  if (pSpecies->getCompartment()->getStatus() != CModelEntity::Status::FIXED)
    {
      Infix << "-";
      Infix << pointerToString(container.getMathObject(pSpecies->getConcentrationReference())->getValuePointer());
      Infix << "*";
      Infix << pointerToString(container.getMathObject(pSpecies->getCompartment()->getRateReference())->getValuePointer());
    }

  Infix << ")/";
  Infix << pointerToString(container.getMathObject(pSpecies->getCompartment()->getValueReference())->getValuePointer());

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("IntensiveRateExpression", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::createExtensiveODERateExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  bool success = true;

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  /*
    mRate = mpModel->getQuantity2NumberFactor() *
      mpCompartment->getValue() * mpExpression->calcValue();
   */

  if (!pSpecies->getExpression().empty())
    {
      Infix << pointerToString(&container.getQuantity2NumberFactor());
      Infix << "*";
      Infix << pointerToString(container.getMathObject(pSpecies->getCompartment()->getValueReference())->getValuePointer());
      Infix << "*(";
      Infix << pSpecies->getExpression();
      Infix << ")";
    }

  CExpression E("ExtensiveODERateExpression", &container);

  success &= E.setInfix(Infix.str());

  pdelete(mpExpression);
  mpExpression = new CMathExpression(E, container, !mIsInitialValue);
  compileExpression();

  return success;
}

bool CMathObject::createExtensiveReactionRateExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  bool success = true;

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(std::numeric_limits<double>::digits10 + 2);

  std::string Key = pSpecies->getKey();
  bool First = true;

  CDataVectorN< CReaction >::const_iterator it = container.getModel().getReactions().begin();
  CDataVectorN< CReaction >::const_iterator end = container.getModel().getReactions().end();

  for (; it != end; ++it)
    {
      const CDataVector< CChemEqElement > &Balances =
        it->getChemEq().getBalances();
      CDataVector< CChemEqElement >::const_iterator itChem = Balances.begin();
      CDataVector< CChemEqElement >::const_iterator endChem = Balances.end();

      for (; itChem != endChem; ++itChem)
        if (itChem->getMetaboliteKey() == Key)
          break;

      if (itChem != endChem)
        {
          const C_FLOAT64 & Multiplicity = itChem->getMultiplicity();

          if (First || Multiplicity < 0.0)
            {
              if (Multiplicity == std::numeric_limits< C_FLOAT64 >::infinity())
                {
                  Infix << "infinity*";
                }
              else if (Multiplicity == -std::numeric_limits< C_FLOAT64 >::infinity())
                {
                  Infix << "-infinity*";
                }
              // Remove multiplying with -1.0
              else if (-1.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() < Multiplicity &&
                       Multiplicity < -1.0 + 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                {
                  Infix << "-";
                }
              // Remove multiplying with 1.0
              else if (1.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() < Multiplicity &&
                       Multiplicity < 1.0 + 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                {
                  // Infix << "+";
                }
              else
                {
                  Infix << Multiplicity << "*";
                }
            }
          else
            {
              if (Multiplicity == std::numeric_limits< C_FLOAT64 >::infinity())
                {
                  Infix << "+infinity*";
                }
              // Remove multiplying with 1.0
              else if (1.0 - 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon() < Multiplicity &&
                       Multiplicity < 1.0 + 100.0 * std::numeric_limits< C_FLOAT64 >::epsilon())
                {
                  Infix << "+";
                }
              else
                {
                  Infix << "+" << Multiplicity << "*";
                }
            }

          First = false;

          Infix << pointerToString(container.getMathObject(it->getParticleFluxReference())->getValuePointer());
        }
    }

  if (mpExpression == NULL)
    {
      mpExpression = new CMathExpression("ExtensiveReactionExpression", container);
    }

  success &= mpExpression->setInfix(Infix.str());
  success &= mpExpression->compile();
  compileExpression();

  return success;
}

bool CMathObject::createIntensiveNoiseExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  // The default value is NaN
  *mpValue = InvalidValue;

  // Reset the prerequisites
  mPrerequisites.clear();
  pdelete(mpExpression);

  bool success = true;

  if (pSpecies->hasNoise())
    {
      mpExpression = new CMathExpression(*pSpecies->getNoiseExpressionPtr(), container, !mIsInitialValue);
    }

  compileExpression();

  return success;
}

bool CMathObject::createExtensiveNoiseExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  // The default value is NaN
  *mpValue = InvalidValue;

  // Reset the prerequisites
  mPrerequisites.clear();
  pdelete(mpExpression);

  bool success = true;

  if (pSpecies->hasNoise())
    {
      /*
        mRate = mpModel->getQuantity2NumberFactor() *
          mpCompartment->getValue() * mpNoiseExpression->calcValue();
       */

      std::string Infix;
      Infix = pointerToString(&container.getQuantity2NumberFactor());
      Infix += "*" + pointerToString(container.getMathObject(pSpecies->getCompartment()->getValueReference())->getValuePointer());
      Infix += "*(" + pSpecies->getNoiseExpression() + ")";

      CExpression E("ExtensiveNoiseExpression", &container);
      success &= E.setInfix(Infix);

      mpExpression = new CMathExpression(E, container, !mIsInitialValue);
    }
  else
    {
      *mpValue = 0.0;
    }

  compileExpression();

  return success;
}

bool CMathObject::createExtensiveReactionNoiseExpression(const CMetab * pSpecies,
    CMathContainer & container)
{
  // The default value is NaN
  *mpValue = InvalidValue;

  // Reset the prerequisites
  mPrerequisites.clear();
  pdelete(mpExpression);

  bool success = true;

  std::ostringstream Infix;
  Infix.imbue(std::locale::classic());
  Infix.precision(16);

  std::string Key = pSpecies->getKey();
  bool First = true;

  CDataVectorN< CReaction >::const_iterator it = container.getModel().getReactions().begin();
  CDataVectorN< CReaction >::const_iterator end = container.getModel().getReactions().end();

  for (; it != end; ++it)
    {
      if (!it->hasNoise()) continue;

      const CDataVector< CChemEqElement > &Balances =
        it->getChemEq().getBalances();
      CDataVector< CChemEqElement >::const_iterator itChem = Balances.begin();
      CDataVector< CChemEqElement >::const_iterator endChem = Balances.end();

      for (; itChem != endChem; ++itChem)
        if (itChem->getMetaboliteKey() == Key)
          break;

      if (itChem != endChem)
        {
          const C_FLOAT64 & Multiplicity = itChem->getMultiplicity();

          if (First || Multiplicity < 0.0)
            {
              if (Multiplicity == std::numeric_limits< C_FLOAT64 >::infinity())
                {
                  Infix << "infinity";
                }
              else if (Multiplicity == -std::numeric_limits< C_FLOAT64 >::infinity())
                {
                  Infix << "-infinity";
                }
              else
                {
                  Infix << Multiplicity;
                }
            }
          else
            {
              if (Multiplicity == std::numeric_limits< C_FLOAT64 >::infinity())
                {
                  Infix << "+infinity";
                }
              else
                {
                  Infix << "+" << Multiplicity;
                }
            }

          First = false;

          Infix << "*";
          Infix << pointerToString(container.getMathObject(it->getParticleNoiseReference())->getValuePointer());
        }
    }

  if (!First)
    {
      mpExpression = new CMathExpression("ExtensiveReactionNoiseExpression", container);
      success &= mpExpression->setInfix(Infix.str());
      success &= mpExpression->compile();
    }
  else
    {
      *mpValue = 0.0;
    }

  compileExpression();

  return success;
}

std::ostream &operator<<(std::ostream &os, const CMathObject & o)
{
  if (o.mpDataObject != NULL)
    {
      os << o.mpDataObject->getCN() << std::endl;
    }
  else
    {
      os << "Data Object = NULL" << std::endl;
    }

  os << "  Pointer:                " << &o << std::endl;
  os << "  Value Type:             ";

  switch (o.mValueType)
    {
      case CMath::ValueType::Undefined:
        os << "ValueTypeUndefined" << std::endl;
        break;

      case CMath::ValueType::Value:
        os << "Value" << std::endl;
        break;

      case CMath::ValueType::Rate:
        os << "ValueRate" << std::endl;
        break;

      case CMath::ValueType::ParticleFlux:
        os << "ParticleFlux" << std::endl;
        break;

      case CMath::ValueType::Flux:
        os << "Flux" << std::endl;
        break;

      case CMath::ValueType::Propensity:
        os << "Propensity" << std::endl;
        break;

      case CMath::ValueType::Noise:
        os << "Noise" << std::endl;
        break;

      case CMath::ValueType::ParticleNoise:
        os << "ParticleNoise" << std::endl;
        break;

      case CMath::ValueType::TotalMass:
        os << "TotalMass" << std::endl;
        break;

      case CMath::ValueType::DependentMass:
        os << "DependentMass" << std::endl;
        break;

      case CMath::ValueType::Discontinuous:
        os << "Discontinuous" << std::endl;
        break;

      case CMath::ValueType::EventDelay:
        os << "EventDelay" << std::endl;
        break;

      case CMath::ValueType::EventPriority:
        os << "EventPriority" << std::endl;
        break;

      case CMath::ValueType::EventAssignment:
        os << "EventAssignment" << std::endl;
        break;

      case CMath::ValueType::EventTrigger:
        os << "EventTrigger" << std::endl;
        break;

      case CMath::ValueType::EventRoot:
        os << "EventRoot" << std::endl;
        break;

      case CMath::ValueType::EventRootState:
        os << "EventRootState" << std::endl;
        break;

      case CMath::ValueType::DelayValue:
        os << "DelayValue" << std::endl;
        break;

      case CMath::ValueType::DelayLag:
        os << "DelayLag" << std::endl;
        break;

      case CMath::ValueType::TransitionTime:
        os << "TransitionTime" << std::endl;
        break;

      case CMath::ValueType::__SIZE:
        os << "***ENUM RANGE" << std::endl;
        break;
    }

  os << "  Simulation Type:        ";

  switch (o.mSimulationType)
    {
      case CMath::SimulationType::Undefined:
        os << "SimulationTypeUndefined" << std::endl;
        break;

      case CMath::SimulationType::Fixed:
        os << "Fixed" << std::endl;
        break;

      case CMath::SimulationType::EventTarget:
        os << "EventTarget" << std::endl;
        break;

      case CMath::SimulationType::Time:
        os << "Time" << std::endl;
        break;

      case CMath::SimulationType::ODE:
        os << "ODE" << std::endl;
        break;

      case CMath::SimulationType::Independent:
        os << "Independent" << std::endl;
        break;

      case CMath::SimulationType::Dependent:
        os << "Dependent" << std::endl;
        break;

      case CMath::SimulationType::Assignment:
        os << "Assignment" << std::endl;
        break;

      case CMath::SimulationType::Conversion:
        os << "Conversion" << std::endl;
        break;

      case CMath::SimulationType::__SIZE:
        os << "***ENUM RANGE" << std::endl;
        break;
    };

  os << "  Entity Type:            ";

  switch (o.mEntityType)
    {
      case CMath::EntityType::Undefined:
        os << "EntityTypeUndefined" << std::endl;
        break;

      case CMath::EntityType::Model:
        os << "Model" << std::endl;
        break;

      case CMath::EntityType::Analysis:
        os << "Analysis" << std::endl;
        break;

      case CMath::EntityType::GlobalQuantity:
        os << "GlobalQuantity" << std::endl;
        break;

      case CMath::EntityType::Compartment:
        os << "Compartment" << std::endl;
        break;

      case CMath::EntityType::Species:
        os << "Species" << std::endl;
        break;

      case CMath::EntityType::LocalReactionParameter:
        os << "LocalReactionParameter" << std::endl;
        break;

      case CMath::EntityType::StoichiometricCoefficients:
        os << "StoichiometricCoefficients" << std::endl;
        break;

      case CMath::EntityType::Reaction:
        os << "Reaction" << std::endl;
        break;

      case CMath::EntityType::Moiety:
        os << "Moiety" << std::endl;
        break;

      case CMath::EntityType::Event:
        os << "Event" << std::endl;
        break;

      case CMath::EntityType::Delay:
        os << "Delay" << std::endl;
        break;

      case CMath::EntityType::__SIZE:
        os << "***ENUM RANGE" << std::endl;
        break;
    };

  os << "  Is Intensive Property:  " << (o.mIsIntensiveProperty ? "true" : "false") << std::endl;

  os << "  Is Initial Value:       " << (o.mIsInitialValue ? "true" : "false") << std::endl;

  os << "  Corresponding Property: ";

  if (o.mpCorrespondingProperty != NULL)
    {
      os << o.mpCorrespondingProperty->getCN() << std::endl;
    }
  else
    {
      os << "NULL" << std::endl;
    }

  os << "  Value:                  " << *o.mpValue << " (" << o.mpValue << ")" << std::endl;

  os << "  Expression:             ";

  if (o.mpExpression != NULL)
    {
      os << o.mpExpression->getRoot()->buildInfix() << std::endl;
    }
  else
    {
      os << "NULL" << std::endl;
    }

  // CObjectInterface::ObjectSet mPrerequisites;

  return os;
}

const CValidity & CMathObject::getValidity() const
{
  return mValidity;
}

// virtual
void CMathObject::validityChanged(const CValidity & changedValidity)
{
  if (mpDataObject != NULL)
    {
      const_cast< CDataObject * >(mpDataObject)->validityChanged(changedValidity);
    }
}
