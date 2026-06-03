import { useTranslation } from 'react-i18next';
import { useLabStore } from './store/labStore';
import Header from './components/Header';
import Navigation from './components/Navigation';
import ParameterPanel from './components/ParameterPanel';
import VisualizationArea from './components/VisualizationArea';
import ResultsPanel from './components/ResultsPanel';
import './App.css';

function App() {
  const { t } = useTranslation();
  const currentLab = useLabStore((state) => state.currentLab);

  return (
    <div className="flex flex-col h-screen w-screen bg-gray-50">
      {/* Header */}
      <Header />

      {/* Lab Navigation */}
      <Navigation />

      {/* Main Content Area */}
      <div className="flex flex-1 gap-4 p-4 overflow-hidden">
        {/* Left Sidebar - Parameters */}
        <div className="w-64 bg-white rounded-lg shadow-sm border border-gray-200 overflow-y-auto">
          <ParameterPanel />
        </div>

        {/* Center - Visualization */}
        <div className="flex-1 bg-white rounded-lg shadow-sm border border-gray-200 overflow-hidden">
          <VisualizationArea />
        </div>

        {/* Right Sidebar - Results */}
        <div className="w-72 bg-white rounded-lg shadow-sm border border-gray-200 overflow-y-auto">
          <ResultsPanel />
        </div>
      </div>

      {/* Instructions Footer */}
      <footer className="bg-white border-t border-gray-200 px-4 py-3 text-sm text-gray-600">
        <p>{t('labs.' + currentLab + '.description')}</p>
      </footer>
    </div>
  );
}

export default App;
