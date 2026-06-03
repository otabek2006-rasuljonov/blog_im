import { useTranslation } from 'react-i18next';

export default function Header() {
  const { t } = useTranslation();

  return (
    <header className="bg-gradient-to-r from-blue-600 to-purple-600 text-white shadow-lg">
      <div className="max-w-full mx-auto px-4 py-4 flex items-center justify-between">
        <div className="flex items-center gap-3">
          <div className="text-2xl">🧪</div>
          <div>
            <h1 className="text-2xl font-bold">{t('app.title')}</h1>
            <p className="text-blue-100 text-sm">{t('app.description')}</p>
          </div>
        </div>
        <div className="text-sm text-blue-100">
          © 2024 Uzlab
        </div>
      </div>
    </header>
  );
}
