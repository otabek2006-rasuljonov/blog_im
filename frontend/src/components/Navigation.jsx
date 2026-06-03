import { useTranslation } from 'react-i18next';
import { useLabStore } from '../store/labStore';

const LABS = ['physics', 'mathematics', 'chemistry', 'biology'];

export default function Navigation() {
  const { t } = useTranslation();
  const { currentLab, setCurrentLab, setCurrentExperiment } = useLabStore();

  const handleLabChange = (lab) => {
    setCurrentLab(lab);
    // Reset to first experiment when changing labs
    const experiments = {
      physics: 'gravity',
      mathematics: 'functions',
      chemistry: 'molecules',
      biology: 'cell'
    };
    setCurrentExperiment(experiments[lab] || 'gravity');
  };

  return (
    <nav className="bg-white border-b border-gray-200 px-4 py-3">
      <div className="flex gap-4">
        {LABS.map((lab) => (
          <button
            key={lab}
            onClick={() => handleLabChange(lab)}
            className={`px-4 py-2 rounded-lg font-medium transition-all ${
              currentLab === lab
                ? 'bg-blue-600 text-white shadow-md'
                : 'bg-gray-100 text-gray-700 hover:bg-gray-200'
            }`}
          >
            {t(`labs.${lab}.title`)}
          </button>
        ))}
      </div>
    </nav>
  );
}
