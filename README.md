# Uzlab - Online Laboratory Platform

**Узлаб** - интерактив онлайн лаборатория платформа, бу ерда ўқувчилар физика, математика, химия ва биология фанларидан виртуал тажрибалар ўтказиши мумкин.

## Лаборатория Тури

### 1. Физика (Fizika)
- Ўзаро тартиб кучи (Gravity) - Планета радиусини ўзгартириб эркин тушиш тезлаштирувчилигини ўзгаришини кузатиш
- Маятник (Pendulum) - Оддий маятник амал-ҳаракати
- Снаряд Ҳаракати (Projectile Motion) - Ҳава муқовамати билан снарядни ишқа сол

### 2. Математика (Matematika)
- Функция график (Function Graphing)
- 3D Бетлар (3D Surface Plotting)
- Геометрик шаклар (Geometric Shapes)

### 3. Химия (Kimyo)
- Молекуларлар 3D кўрувчи (Molecular Viewer)
- Реакция суръати (Reaction Rates)
- Материя Ҳолати (States of Matter)

### 4. Биология (Biologiya)
- Клетка 3D Модели (Cell 3D Model)
- Фотосинтез (Photosynthesis)
- ДНК репликация (DNA Replication)

## Тинмаси

### Frontend
```bash
cd frontend
npm install
npm run dev
```

### Backend
```bash
python manage.py runserver
```

## Poyvaznov Framework

**Frontend:**
- React + Vite
- Three.js для 3D визуализации
- Tailwind CSS для дизайна
- Zustand для состояния
- i18next для локализации

**Backend:**
- Django + Django REST Framework
- PostgreSQL база данных
- Redis для кеша

## Структура Пројекта

```
blog_im/
├── frontend/              # React приложение
│   ├── src/
│   │   ├── components/   # React компоненты
│   │   ├── simulations/  # Симуляционные классы
│   │   ├── store/        # Zustand хранилище
│   │   └── locales/      # Переводы (Uzbek)
│   ├── package.json
│   └── vite.config.js
├── backend/              # Django приложение
│   ├── manage.py
│   ├── requirements.txt
│   └── ...
└── README.md
```

## Функциональные Возможности

✓ **Интерактивные Симуляции** - Real-time 3D визуализация
✓ **Параметры Управления** - Ползунки для изменения параметров
✓ **Вычисление Результатов** - Автоматические расчеты физических формул
✓ **Локализация** - Полный интерфейс на узбекском языке
✓ **Мобильная Поддержка** - Работает на смартфонах и планшетах
✓ **Производительность** - Оптимизировано для быстрой загрузки

## Фазы Разработки

- [x] **Фаза 1: Фундамент** - React, Tailwind, Three.js, i18n
- [x] **Фаза 2: Физика** - Gravity, Pendulum, Projectile simulations
- [ ] **Фаза 3: Математика** - Graphing, surfaces, geometry
- [ ] **Фаза 4: Химия** - Molecular viewer, reactions
- [ ] **Фаза 5: Биология** - Cell models, processes
- [ ] **Фаза 6: Оптимизация** - Performance, PWA, deployment

## Языковая Поддержка

- 🇺🇿 Узбекский (Cyrillic)
- Расширяемая архитектура для других языков

## Браузерная Поддержка

- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## Лицензия

MIT License

## Автор

Создано для образовательных учреждений Узбекистана