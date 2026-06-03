import { useEffect, useRef } from 'react';
import { useLabStore } from '../store/labStore';
import GravitySimulation from '../simulations/physics/GravitySimulation';
import PendulumSimulation from '../simulations/physics/PendulumSimulation';
import ProjectileSimulation from '../simulations/physics/ProjectileSimulation';
import FunctionGraphing from '../simulations/mathematics/FunctionGraphing';
import GeometryShapes from '../simulations/mathematics/GeometryShapes';
import MolecularViewer from '../simulations/chemistry/MolecularViewer';
import CellModel from '../simulations/biology/CellModel';

export default function VisualizationArea() {
  const containerRef = useRef(null);
  const simulationRef = useRef(null);
  
  const { currentLab, currentExperiment, physicsParams, mathParams, chemistryParams, biologyParams, isRunning } = useLabStore();

  useEffect(() => {
    if (!containerRef.current) return;

    // Clean up previous simulation
    if (simulationRef.current) {
      simulationRef.current.dispose?.();
    }

    // Create new simulation
    if (currentLab === 'physics') {
      if (currentExperiment === 'gravity') {
        simulationRef.current = new GravitySimulation(
          containerRef.current,
          physicsParams.gravity
        );
      } else if (currentExperiment === 'pendulum') {
        simulationRef.current = new PendulumSimulation(
          containerRef.current,
          physicsParams.pendulum
        );
      } else if (currentExperiment === 'projectile') {
        simulationRef.current = new ProjectileSimulation(
          containerRef.current,
          physicsParams.projectile
        );
      }
    } else if (currentLab === 'mathematics') {
      if (currentExperiment === 'functions') {
        simulationRef.current = new FunctionGraphing(
          containerRef.current,
          mathParams.functions
        );
      } else if (currentExperiment === 'geometry') {
        simulationRef.current = new GeometryShapes(
          containerRef.current,
          mathParams.geometry
        );
      }
    } else if (currentLab === 'chemistry') {
      if (currentExperiment === 'molecules') {
        simulationRef.current = new MolecularViewer(
          containerRef.current,
          chemistryParams.molecules
        );
      }
    } else if (currentLab === 'biology') {
      if (currentExperiment === 'cell') {
        simulationRef.current = new CellModel(
          containerRef.current,
          biologyParams.cell
        );
      }
    }

    return () => {
      if (simulationRef.current?.dispose) {
        simulationRef.current.dispose();
      }
    };
  }, [currentLab, currentExperiment, containerRef]);

  // Update simulation when parameters change
  useEffect(() => {
    if (simulationRef.current?.updateParameters) {
      if (currentLab === 'physics') {
        if (currentExperiment === 'gravity') {
          simulationRef.current.updateParameters(physicsParams.gravity);
        } else if (currentExperiment === 'pendulum') {
          simulationRef.current.updateParameters(physicsParams.pendulum);
        } else if (currentExperiment === 'projectile') {
          simulationRef.current.updateParameters(physicsParams.projectile);
        }
      } else if (currentLab === 'mathematics') {
        if (currentExperiment === 'functions') {
          simulationRef.current.updateParameters(mathParams.functions);
        } else if (currentExperiment === 'geometry') {
          simulationRef.current.updateParameters(mathParams.geometry);
        }
      } else if (currentLab === 'chemistry') {
        if (currentExperiment === 'molecules') {
          simulationRef.current.updateParameters(chemistryParams.molecules);
        }
      } else if (currentLab === 'biology') {
        if (currentExperiment === 'cell') {
          simulationRef.current.updateParameters(biologyParams.cell);
        }
      }
    }
  }, [physicsParams, mathParams, chemistryParams, biologyParams, currentExperiment]);

  // Control simulation play/pause (for physics only)
  useEffect(() => {
    if (currentLab === 'physics' && simulationRef.current) {
      if (isRunning) {
        simulationRef.current.start?.();
      } else {
        simulationRef.current.stop?.();
      }
    }
  }, [isRunning, currentLab]);

  return (
    <div
      ref={containerRef}
      className="w-full h-full bg-gradient-to-b from-blue-50 to-purple-50 relative"
    >
      {/* Placeholder text while simulation loads */}
      <div className="absolute inset-0 flex items-center justify-center text-gray-400 pointer-events-none">
        <p>Симуляция юклинмоқда...</p>
      </div>
    </div>
  );
}
