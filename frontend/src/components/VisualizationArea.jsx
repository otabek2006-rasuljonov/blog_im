import { useEffect, useRef } from 'react';
import { useLabStore } from '../store/labStore';
import GravitySimulation from '../simulations/physics/GravitySimulation';
import PendulumSimulation from '../simulations/physics/PendulumSimulation';

export default function VisualizationArea() {
  const containerRef = useRef(null);
  const simulationRef = useRef(null);
  
  const { currentLab, currentExperiment, physicsParams, isRunning } = useLabStore();

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
      if (currentExperiment === 'gravity') {
        simulationRef.current.updateParameters(physicsParams.gravity);
      } else if (currentExperiment === 'pendulum') {
        simulationRef.current.updateParameters(physicsParams.pendulum);
      }
    }
  }, [physicsParams, currentExperiment]);

  // Control simulation play/pause
  useEffect(() => {
    if (simulationRef.current) {
      if (isRunning) {
        simulationRef.current.start?.();
      } else {
        simulationRef.current.stop?.();
      }
    }
  }, [isRunning]);

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
