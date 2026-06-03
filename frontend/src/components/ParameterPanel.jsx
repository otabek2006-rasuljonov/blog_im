import { useTranslation } from 'react-i18next';
import { useLabStore } from '../store/labStore';
import SliderControl from './controls/SliderControl';

const EXPERIMENTS = {
  physics: ['gravity', 'pendulum', 'projectile'],
  mathematics: ['functions', 'geometry'],
  chemistry: ['molecules', 'reactions', 'matter'],
  biology: ['cell', 'photosynthesis', 'dna']
};

const FUNCTION_TYPES = ['sine', 'cosine', 'tangent', 'quadratic', 'cubic', 'exponential', 'logarithm'];
const GEOMETRY_SHAPES = ['cube', 'sphere', 'cylinder', 'cone', 'torus', 'tetrahedron', 'octahedron', 'dodecahedron', 'icosahedron'];
const MOLECULES = ['water', 'methane', 'carbonDioxide', 'ammonia'];

export default function ParameterPanel() {
  const { t } = useTranslation();
  const { 
    currentLab, 
    currentExperiment, 
    setCurrentExperiment,
    physicsParams, 
    updatePhysicsParam,
    mathParams,
    updateMathParam,
    chemistryParams,
    updateChemistryParam,
    setIsRunning 
  } = useLabStore();

  const experiments = EXPERIMENTS[currentLab] || [];

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

      case 'projectile':
        return (
          <div className="space-y-4">
            <h3 className="text-lg font-semibold text-gray-800">{t('physics.projectile.title')}</h3>
            
            <SliderControl
              label={t('physics.projectile.velocity')}
              value={params.velocity}
              min={5}
              max={100}
              step={1}
              onChange={(value) => updatePhysicsParam('projectile', 'velocity', value)}
              unit={t('units.metersPerSecond')}
            />
            
            <SliderControl
              label={t('physics.projectile.angle')}
              value={params.angle}
              min={0}
              max={90}
              step={1}
              onChange={(value) => updatePhysicsParam('projectile', 'angle', value)}
              unit={t('units.degrees')}
            />

            <label className="flex items-center space-x-2 cursor-pointer">
              <input
                type="checkbox"
                checked={params.airResistance}
                onChange={(e) => updatePhysicsParam('projectile', 'airResistance', e.target.checked)}
                className="w-4 h-4 accent-blue-600"
              />
              <span className="text-sm font-medium text-gray-700">{t('physics.projectile.airResistance')}</span>
            </label>

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

  const renderMathematicsControls = () => {
    switch (currentExperiment) {
      case 'functions':
        const funcParams = mathParams.functions;
        return (
          <div className="space-y-4">
            <h3 className="text-lg font-semibold text-gray-800">{t('mathematics.functions.title')}</h3>
            
            <div className="space-y-2">
              <label className="text-sm font-medium text-gray-700">{t('mathematics.functions.type')}</label>
              <select
                value={funcParams.type}
                onChange={(e) => updateMathParam('functions', 'type', e.target.value)}
                className="w-full px-3 py-2 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500"
              >
                {FUNCTION_TYPES.map(type => (
                  <option key={type} value={type}>{type}</option>
                ))}
              </select>
            </div>
            
            <SliderControl
              label={t('mathematics.functions.amplitude')}
              value={funcParams.amplitude}
              min={0.1}
              max={5}
              step={0.1}
              onChange={(value) => updateMathParam('functions', 'amplitude', value)}
              unit=""
            />
            
            <SliderControl
              label={t('mathematics.functions.frequency')}
              value={funcParams.frequency}
              min={0.1}
              max={5}
              step={0.1}
              onChange={(value) => updateMathParam('functions', 'frequency', value)}
              unit=""
            />

            <SliderControl
              label={t('mathematics.functions.phase')}
              value={funcParams.phase}
              min={0}
              max={Math.PI * 2}
              step={0.1}
              onChange={(value) => updateMathParam('functions', 'phase', value)}
              unit="rad"
            />
          </div>
        );

      case 'geometry':
        const geoParams = mathParams.geometry;
        return (
          <div className="space-y-4">
            <h3 className="text-lg font-semibold text-gray-800">{t('mathematics.geometry.title')}</h3>
            
            <div className="space-y-2">
              <label className="text-sm font-medium text-gray-700">{t('mathematics.geometry.type')}</label>
              <select
                value={geoParams.type}
                onChange={(e) => updateMathParam('geometry', 'type', e.target.value)}
                className="w-full px-3 py-2 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500"
              >
                {GEOMETRY_SHAPES.map(shape => (
                  <option key={shape} value={shape}>{shape}</option>
                ))}
              </select>
            </div>
            
            <SliderControl
              label={t('mathematics.geometry.size')}
              value={geoParams.size}
              min={5}
              max={50}
              step={1}
              onChange={(value) => updateMathParam('geometry', 'size', value)}
              unit="unit"
            />

            {geoParams.volume > 0 && (
              <div className="bg-gray-50 p-3 rounded-lg">
                <p className="text-sm text-gray-600">{t('mathematics.geometry.volume')}</p>
                <p className="text-lg font-semibold text-blue-600">
                  {geoParams.volume.toFixed(2)} unit³
                </p>
              </div>
            )}
          </div>
        );
      
      default:
        return <div className="text-gray-600">{t('controls.presets')}</div>;
    }
  };

  const renderChemistryControls = () => {
    switch (currentExperiment) {
      case 'molecules':
        const molParams = chemistryParams.molecules;
        return (
          <div className="space-y-4">
            <h3 className="text-lg font-semibold text-gray-800">Молекуллар</h3>
            
            <div className="space-y-2">
              <label className="text-sm font-medium text-gray-700">Молекула Тури</label>
              <select
                value={molParams.type}
                onChange={(e) => updateChemistryParam('molecules', 'type', e.target.value)}
                className="w-full px-3 py-2 border border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500"
              >
                {MOLECULES.map(mol => (
                  <option key={mol} value={mol}>{mol}</option>
                ))}
              </select>
            </div>

            <div className="bg-blue-50 p-3 rounded-lg">
              <p className="text-sm text-gray-600">Ротация</p>
              <p className="text-xs text-gray-500">Сичкамни сусувчи билан суғириш</p>
            </div>
          </div>
        );
      
      default:
        return <div className="text-gray-600">{t('controls.presets')}</div>;
    }
  };

  return (
    <div className="p-4">
      <h2 className="text-lg font-bold text-gray-800 mb-4">
        {t('controls.presets')}
      </h2>

      {/* Experiment Selection Tabs */}
      {experiments.length > 0 && (
        <div className="mb-6 space-y-2">
          {experiments.map((exp) => (
            <button
              key={exp}
              onClick={() => setCurrentExperiment(exp)}
              className={`w-full px-3 py-2 rounded-lg font-medium text-sm transition-all ${
                currentExperiment === exp
                  ? 'bg-blue-600 text-white shadow-md'
                  : 'bg-gray-100 text-gray-700 hover:bg-gray-200'
              }`}
            >
              {currentLab === 'physics' && t(`physics.${exp}.title`)}
              {currentLab === 'mathematics' && t(`mathematics.${exp}.title`)}
              {currentLab === 'chemistry' && `${exp}`}
              {currentLab === 'biology' && `${exp}`}
            </button>
          ))}
        </div>
      )}

      {/* Controls */}
      {currentLab === 'physics' && renderPhysicsControls()}
      {currentLab === 'mathematics' && renderMathematicsControls()}
      {currentLab === 'chemistry' && renderChemistryControls()}
      {currentLab === 'biology' && (
        <div className="text-gray-500 text-center py-8">
          Тез ороқа аст
        </div>
      )}
    </div>
  );
}
