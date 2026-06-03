import { create } from 'zustand';

export const useLabStore = create((set) => ({
  // Current selected lab
  currentLab: 'physics',
  setCurrentLab: (lab) => set({ currentLab: lab }),

  // Current selected experiment
  currentExperiment: 'gravity',
  setCurrentExperiment: (experiment) => set({ currentExperiment: experiment }),

  // Physics lab parameters
  physicsParams: {
    gravity: {
      planetRadius: 6371,
      planetMass: 5.972,
      height: 100,
      velocity: 0,
      acceleration: 9.81,
      time: 0
    },
    pendulum: {
      length: 1.0,
      angle: 30,
      mass: 1.0,
      gravity: 9.81,
      period: 0
    },
    projectile: {
      velocity: 20,
      angle: 45,
      airResistance: false,
      range: 0,
      height: 0
    }
  },

  // Mathematics lab parameters
  mathParams: {
    functions: {
      type: 'sine',
      amplitude: 1,
      frequency: 1,
      phase: 0
    },
    geometry: {
      type: 'cube',
      size: 20,
      volume: 0,
      surfaceArea: 0
    }
  },

  // Chemistry lab parameters
  chemistryParams: {
    molecules: {
      type: 'water'
    },
    reactions: {
      temperature: 298.15,
      pressure: 1.0,
      concentration: 1.0
    },
    matter: {
      type: 'solid',
      temperature: 273.15
    }
  },

  // Biology lab parameters
  biologyParams: {
    cell: {
      type: 'animal'
    },
    photosynthesis: {
      lightIntensity: 1.0,
      co2Level: 1.0,
      temperature: 298.15
    },
    dna: {
      sequence: 'ATGC'
    }
  },

  updatePhysicsParam: (experiment, paramName, value) => 
    set((state) => ({
      physicsParams: {
        ...state.physicsParams,
        [experiment]: {
          ...state.physicsParams[experiment],
          [paramName]: value
        }
      }
    })),

  updateMathParam: (experiment, paramName, value) =>
    set((state) => ({
      mathParams: {
        ...state.mathParams,
        [experiment]: {
          ...state.mathParams[experiment],
          [paramName]: value
        }
      }
    })),

  updateChemistryParam: (experiment, paramName, value) =>
    set((state) => ({
      chemistryParams: {
        ...state.chemistryParams,
        [experiment]: {
          ...state.chemistryParams[experiment],
          [paramName]: value
        }
      }
    })),

  updateBiologyParam: (experiment, paramName, value) =>
    set((state) => ({
      biologyParams: {
        ...state.biologyParams,
        [experiment]: {
          ...state.biologyParams[experiment],
          [paramName]: value
        }
      }
    })),

  resetPhysicsParams: (experiment) =>
    set((state) => ({
      physicsParams: {
        ...state.physicsParams,
        [experiment]: state.physicsParams[experiment]
      }
    })),

  // Simulation state
  isRunning: false,
  setIsRunning: (running) => set({ isRunning: running }),

  // Results
  results: {},
  setResults: (results) => set({ results }),
}));
