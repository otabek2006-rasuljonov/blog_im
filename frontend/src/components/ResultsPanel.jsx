import { useTranslation } from 'react-i18next';
import { useLabStore } from '../store/labStore';

export default function ResultsPanel() {
  const { t } = useTranslation();
  const { currentLab, currentExperiment, physicsParams, results } = useLabStore();

  const renderResults = () => {
    if (currentLab === 'physics' && currentExperiment === 'gravity') {
      const params = physicsParams.gravity;
      
      // Gravity calculation: G * M / r^2
      const G = 6.674e-11;
      const planetMass = params.planetMass * 1e24;
      const planetRadiusM = params.planetRadius * 1000;
      const acceleration = (G * planetMass) / (planetRadiusM * planetRadiusM);
      
      // Free fall calculations
      const vf = Math.sqrt(2 * acceleration * params.height);
      const time = vf / acceleration;

      return (
        <div className="space-y-4">
          <h3 className="text-lg font-semibold text-gray-800">
            {t('physics.gravity.results')}
          </h3>
          
          <div className="space-y-3">
            <ResultItem
              label={t('physics.gravity.acceleration')}
              value={acceleration.toFixed(4)}
              unit={t('units.metersPerSecondSquared')}
            />
            <ResultItem
              label={t('physics.gravity.velocity')}
              value={vf.toFixed(2)}
              unit={t('units.metersPerSecond')}
            />
            <ResultItem
              label={t('physics.gravity.time')}
              value={time.toFixed(2)}
              unit={t('units.seconds')}
            />
          </div>
        </div>
      );
    }

    if (currentLab === 'physics' && currentExperiment === 'pendulum') {
      const params = physicsParams.pendulum;
      
      // Period of pendulum: T = 2π * sqrt(L/g)
      const period = 2 * Math.PI * Math.sqrt(params.length / params.gravity);

      return (
        <div className="space-y-4">
          <h3 className="text-lg font-semibold text-gray-800">
            {t('physics.pendulum.title')} - {t('physics.gravity.results')}
          </h3>
          
          <div className="space-y-3">
            <ResultItem
              label={t('physics.pendulum.period')}
              value={period.toFixed(4)}
              unit={t('units.seconds')}
            />
            <ResultItem
              label="Частота"
              value={(1 / period).toFixed(4)}
              unit="Hz"
            />
          </div>
        </div>
      );
    }

    return (
      <div className="text-gray-500 text-center py-8">
        Натижалар юқори чиқади
      </div>
    );
  };

  return (
    <div className="p-4">
      <h2 className="text-xl font-bold text-gray-800 mb-6">
        {t('physics.gravity.results')}
      </h2>
      {renderResults()}
    </div>
  );
}

function ResultItem({ label, value, unit }) {
  return (
    <div className="bg-gray-50 p-3 rounded-lg">
      <p className="text-sm text-gray-600">{label}</p>
      <p className="text-lg font-semibold text-blue-600">
        {value} <span className="text-sm text-gray-500">{unit}</span>
      </p>
    </div>
  );
}
