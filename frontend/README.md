# Uzlab - Online Laboratory Platform

An interactive, web-based laboratory platform for students to conduct virtual science experiments in Physics, Mathematics, Chemistry, and Biology.

## Features

### Physics Laboratory
- **Gravity Simulation**: Adjust planet radius and mass to observe gravitational acceleration changes
- **Pendulum Motion**: Observe simple harmonic motion with adjustable length and amplitude
- **Projectile Motion**: Launch projectiles with adjustable velocity and angle, with optional air resistance

### User Interface
- **Responsive Design**: Fully responsive layout that works on desktop, tablet, and mobile devices
- **Real-time Simulations**: 3D visualizations using Three.js for immersive learning
- **Interactive Parameters**: Adjust experiment parameters with sliders and see real-time results
- **Results Panel**: Display calculations and physics formulas relevant to the experiment

### Localization
- **Uzbek Language**: Complete interface in Uzbek (Cyrillic script)
- **Scientific Terminology**: Accurate translations of scientific concepts

## Project Structure

```
frontend/
├── src/
│   ├── components/          # React components
│   │   ├── Header.jsx      # Application header
│   │   ├── Navigation.jsx  # Lab selection
│   │   ├── ParameterPanel.jsx # Parameter controls
│   │   ├── VisualizationArea.jsx # 3D visualization
│   │   ├── ResultsPanel.jsx # Results display
│   │   └── controls/       # Control components
│   ├── simulations/        # Simulation classes
│   │   └── physics/        # Physics simulations
│   ├── store/              # Zustand state management
│   ├── locales/            # i18n translations
│   └── App.jsx             # Main component
├── package.json
├── vite.config.js
├── tailwind.config.js
└── postcss.config.js
```

## Technology Stack

### Frontend
- **React** - UI framework
- **Vite** - Build tool and dev server
- **Three.js** - 3D graphics and visualization
- **Tailwind CSS** - Styling and responsive design
- **Zustand** - State management
- **i18next** - Internationalization

### Development
- **Node.js** - JavaScript runtime
- **npm** - Package manager

## Getting Started

### Prerequisites
- Node.js 16+ 
- npm 7+

### Installation

```bash
cd frontend
npm install
```

### Development

```bash
npm run dev
```

The application will start at `http://localhost:5173`

### Building for Production

```bash
npm run build
```

The optimized production build will be in the `dist/` directory.

## Usage

1. **Select a Laboratory**: Use the navigation bar to choose Physics, Mathematics, Chemistry, or Biology
2. **Choose an Experiment**: Select from available experiments in the left panel
3. **Adjust Parameters**: Use sliders to modify experiment parameters
4. **Run Simulation**: Click "Бошлаш" (Play) to start the simulation
5. **View Results**: Check the right panel for calculated results and formulas

## Physics Experiments

### Gravity Simulation
**Concepts**: Gravitational acceleration, free fall, planetary mass and radius

**Parameters**:
- Planet Radius (1000-20000 km)
- Planet Mass (1-20 × 10²⁴ kg)
- Drop Height (0-10000 m)

**Results**:
- Gravitational acceleration (m/s²)
- Final velocity (m/s)
- Time to fall (s)

### Pendulum Motion
**Concepts**: Simple harmonic motion, oscillation period, angular frequency

**Parameters**:
- String Length (0.1-5 m)
- Initial Angle (0-90°)

**Results**:
- Period of oscillation (s)
- Frequency (Hz)

### Projectile Motion
**Concepts**: Projectile trajectory, range, maximum height, air resistance

**Parameters**:
- Initial Velocity (5-100 m/s)
- Launch Angle (0-90°)
- Air Resistance (on/off)

**Results**:
- Range (m)
- Maximum Height (m)
- Time of Flight (s)

## Performance Optimization

- **Code Splitting**: Simulations are loaded on demand
- **GPU Acceleration**: Three.js uses GPU for rendering
- **Responsive Rendering**: Adaptive quality based on device capabilities
- **Optimized 3D Models**: Decimated meshes for mobile devices

## Browser Support

- Chrome/Chromium 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## Accessibility

- Keyboard navigation support
- High contrast color schemes
- Alt text for all visual elements
- Semantic HTML structure

## Future Enhancements

### Phase 3: Mathematics Laboratory
- Function graphing with parameter adjustment
- 3D surface plotting
- Geometric shape visualizations

### Phase 4: Chemistry Laboratory
- Molecular 3D structure viewer
- Reaction rate simulator
- State of matter transitions

### Phase 5: Biology Laboratory
- Cell 3D model with zoomable components
- Process animations
- Microscope simulator

### Phase 6: Optimization & Deployment
- Performance optimization
- Mobile testing
- Service workers for offline capability
- Progressive Web App (PWA) support

## Contributing

Contributions are welcome! Please ensure all code follows the existing style and includes appropriate comments.

## License

This project is licensed under the MIT License.

## Support

For issues, questions, or suggestions, please open an issue on the project repository.

## Credits

Created for Uzbek educational institutions to provide interactive, hands-on learning experiences in science education.
