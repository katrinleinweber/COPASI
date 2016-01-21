// Copyright (C) 2011 - 2016 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

#ifndef COPASI_CMathEnum
#define COPASI_CMathEnum

#include <map>
#include <vector>

class CCopasiObject;
class CMathObject;
class CEvaluationNode;
class CObjectInterface;

template <class Enum> class CMathFlags
{
public:
  CMathFlags():
    mFlags(0)
  {}

  CMathFlags(const CMathFlags & src):
    mFlags(src.mFlags)
  {}

  CMathFlags(const Enum & flag):
    mFlags((int) flag)
  {}

  CMathFlags(const int & flag):
    mFlags(flag)
  {}

  ~CMathFlags() {}

  operator int () const
  {
    return mFlags;
  }

  Enum operator & (const Enum & flag) const
  {
    return (Enum)(mFlags & flag);
  }

  CMathFlags operator & (const CMathFlags & mask) const
  {
    return (mFlags & mask);
  }

  CMathFlags operator & (const int & mask) const
  {
    return (mFlags & mask);
  }

  CMathFlags & operator = (const CMathFlags & rhs)
  {
    mFlags = rhs.mFlags;
    return *this;
  }

  CMathFlags operator | (const Enum & flag) const
  {
    return (mFlags | flag);
  }

  CMathFlags operator | (const CMathFlags & other) const
  {
    return (mFlags | other);
  }

  CMathFlags & operator |= (const CMathFlags & other)
  {
    mFlags |= other.mFlags;
    return *this;
  }

private:
  int mFlags;
};

class CMath
{
public:
  typedef std::multimap< std::string, std::pair< std::string, CMathObject * > > DelayValueData;
  typedef std::multimap< std::string, DelayValueData > DelayData;

  struct sPointers
  {
  public:
    C_FLOAT64 * pInitialExtensiveValues;
    C_FLOAT64 * pInitialIntensiveValues;
    C_FLOAT64 * pInitialExtensiveRates;
    C_FLOAT64 * pInitialIntensiveRates;
    C_FLOAT64 * pInitialParticleFluxes;
    C_FLOAT64 * pInitialFluxes;
    C_FLOAT64 * pInitialTotalMasses;
    C_FLOAT64 * pInitialEventTriggers;

    C_FLOAT64 * pExtensiveValues;
    C_FLOAT64 * pIntensiveValues;
    C_FLOAT64 * pExtensiveRates;
    C_FLOAT64 * pIntensiveRates;
    C_FLOAT64 * pParticleFluxes;
    C_FLOAT64 * pFluxes;
    C_FLOAT64 * pTotalMasses;
    C_FLOAT64 * pEventTriggers;

    C_FLOAT64 * pEventDelays;
    C_FLOAT64 * pEventPriorities;
    C_FLOAT64 * pEventAssignments;
    C_FLOAT64 * pEventRoots;
    C_FLOAT64 * pEventRootStates;
    C_FLOAT64 * pPropensities;
    C_FLOAT64 * pDependentMasses;
    C_FLOAT64 * pDiscontinuous;
    C_FLOAT64 * pDelayValue;
    C_FLOAT64 * pDelayLag;
    C_FLOAT64 * pTransitionTime;

    CMathObject * pInitialExtensiveValuesObject;
    CMathObject * pInitialIntensiveValuesObject;
    CMathObject * pInitialExtensiveRatesObject;
    CMathObject * pInitialIntensiveRatesObject;
    CMathObject * pInitialParticleFluxesObject;
    CMathObject * pInitialFluxesObject;
    CMathObject * pInitialTotalMassesObject;
    CMathObject * pInitialEventTriggersObject;

    CMathObject * pExtensiveValuesObject;
    CMathObject * pIntensiveValuesObject;
    CMathObject * pExtensiveRatesObject;
    CMathObject * pIntensiveRatesObject;
    CMathObject * pParticleFluxesObject;
    CMathObject * pFluxesObject;
    CMathObject * pTotalMassesObject;
    CMathObject * pEventTriggersObject;

    CMathObject * pEventDelaysObject;
    CMathObject * pEventPrioritiesObject;
    CMathObject * pEventAssignmentsObject;
    CMathObject * pEventRootsObject;
    CMathObject * pEventRootStatesObject;
    CMathObject * pPropensitiesObject;
    CMathObject * pDependentMassesObject;
    CMathObject * pDiscontinuousObject;
    CMathObject * pDelayValueObject;
    CMathObject * pDelayLagObject;
    CMathObject * pTransitionTimeObject;
  };

  struct sRelocate
  {
  public:
    C_FLOAT64 * pValueStart;
    C_FLOAT64 * pValueEnd;
    C_FLOAT64 * pOldValue;
    C_FLOAT64 * pNewValue;

    CMathObject * pObjectStart;
    CMathObject * pObjectEnd;
    CMathObject * pOldObject;
    CMathObject * pNewObject;

    size_t offset;
  };

  template < class CType > class Entity
  {
  public:
    CType * InitialValue;
    CType * InitialRate;
    CType * Value;
    CType * Rate;

    Entity()
      : InitialValue(NULL)
      , InitialRate(NULL)
      , Value(NULL)
      , Rate(NULL)
    {}
  };

  enum SimulationContext
  {
    Default = 0x0,
    // This is used to indicate deterministic simulation
    // Deterministic = 0x1,
    // This must be set when using the reduced model
    UseMoieties = 0x2,
    // This updates the total mass of a moiety and must be set
    // at the beginning of the simulation or after events
    UpdateMoieties = 0x4,
    // This is used to indicate stochastic simulation
    // Stochastic = 0x8,
    // Event handling
    EventHandling = 0x10,
    // This is used to detect whether a delay value depends on other delay values.
    DelayValues = 0x20
  };

  typedef CMathFlags< SimulationContext > SimulationContextFlag;

  enum ValueType
  {
    ValueTypeUndefined,
    Value,
    Rate,
    ParticleFlux,
    Flux,
    Propensity,
    TotalMass,
    DependentMass,
    Discontinuous,
    EventDelay,
    EventPriority,
    EventAssignment,
    EventTrigger,
    EventRoot,
    EventRootState,
    DelayValue,
    DelayLag,
    TransitionTime
  };

  enum SimulationType
  {
    SimulationTypeUndefined,
    Fixed,
    EventTarget,
    Time,
    ODE,
    Independent,
    Dependent,
    Assignment,
    Conversion
  };

  enum EntityType
  {
    EntityTypeUndefined,
    Model,
    Analysis,
    GlobalQuantity,
    Compartment,
    Species,
    LocalReactionParameter,
    StoichiometricCoefficients,
    Reaction,
    Moiety,
    Event,
    Delay
  };

  enum eStateChange
  {
    NoChange = 0x0,
    State = 0x1,
    EventSimulation = 0x2,
    ContinuousSimulation = 0x4
  };

  enum RootToggleType
  {
    NoToggle = 0x0,
    ToggleBoth = 0x1,
    ToggleEquality = 0x2,
    ToggleInequality = 0x3
  };

  typedef CMathFlags< eStateChange > StateChange;

  template < class Type > class Variables: public std::vector< Type >
  {
  public:
    Variables():
      std::vector< Type >()
    {}

    Variables(const std::vector< Type > & src):
      std::vector< Type >(src)
    {}

    ~Variables()
    {}
  };
};

#endif // COPASI_CMathEnum
