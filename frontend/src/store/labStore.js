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
