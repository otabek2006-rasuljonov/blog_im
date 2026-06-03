import { useTranslation } from 'react-i18next';
import { useLabStore } from '../store/labStore';
import SliderControl from './controls/SliderControl';

export default function ParameterPanel() {
  const { t } = useTranslation();
  const { currentLab, currentExperiment, physicsParams, updatePhysicsParam, setIsRunning } = useLabStore();

  const renderPhysicsControls = () => {
    const params = physicsParams[currentExperiment];
    
    switch (currentExperiment) {
      case 'gravity':
        return (
          <div className="space-y-4">
            <h3 className="text-lg font-semibold text-gray-800">{t('physics.gravity.title')}</h3>
            
            <SliderControl
              label={t('physics.gravity.planetRadius')}
              value={params.planetRadius}
              min={1000}
              max={20000}
              step={100}
              onChange={(value) => updatePhysicsParam('gravity', 'planetRadius', value)}
              unit={t('units.kilometers')}
            />
            
            <SliderControl
              label={t('physics.gravity.planetMass')}
              value={params.planetMass}
              min={1}
              max={20}
              step={0.1}
              onChange={(value) => updatePhysicsParam('gravity', 'planetMass', value)}
              unit="10²⁴ kg"
            />
            
            <SliderControl
              label={t('physics.gravity.height')}
              value={params.height}
              min={0}
              max={10000}
              step={10}
              onChange={(value) => updatePhysicsParam('gravity', 'height', value)}
              unit={t('units.meters')}
            />

            <button
              onClick={() => setIsRunning(true)}
              className="w-full bg-blue-600 hover:bg-blue-700 text-white font-semibold py-2 px-4 rounded-lg transition"
            >
              {t('controls.play')}
            </button>
          </div>
        );
      
      case 'pendulum':
        return (
          <div className="space-y-4">
            <h3 className="text-lg font-semibold text-gray-800">{t('physics.pendulum.title')}</h3>
            
            <SliderControl
              label={t('physics.pendulum.length')}
              value={params.length}
              min={0.1}
              max={5}
              step={0.1}
              onChange={(value) => updatePhysicsParam('pendulum', 'length', value)}
              unit={t('units.meters')}
            />
            
            <SliderControl
              label={t('physics.pendulum.angle')}
              value={params.angle}
              min={0}
              max={90}
              step={1}
              onChange={(value) => updatePhysicsParam('pendulum', 'angle', value)}
              unit={t('units.degrees')}
            />

            <button
              onClick={() => setIsRunning(true)}
              className="w-full bg-blue-600 hover:bg-blue-700 text-white font-semibold py-2 px-4 rounded-lg transition"
            >
              {t('controls.play')}
            </button>
          </div>
        );
      
      default:
        return <div className="text-gray-600">{t('controls.presets')}</div>;
    }
  };

  return (
    <div className="p-4">
      <h2 className="text-xl font-bold text-gray-800 mb-6">
        {t('controls.presets')}
      </h2>
      
      {currentLab === 'physics' && renderPhysicsControls()}
      {currentLab !== 'physics' && (
        <div className="text-gray-500 text-center py-8">
          Тез ороқа аст
        </div>
      )}
    </div>
  );
}
