import * as THREE from 'three';

export default class GravitySimulation {
  constructor(container, params) {
    this.container = container;
    this.params = params;
    this.isRunning = false;
    this.elapsed = 0;
    this.animationId = null;

    this.init();
  }

  init() {
    // Scene setup
    this.scene = new THREE.Scene();
    this.scene.background = new THREE.Color(0xf0f4f8);

    // Camera setup
    this.camera = new THREE.PerspectiveCamera(
      75,
      this.container.clientWidth / this.container.clientHeight,
      0.1,
      10000
    );
    this.camera.position.set(0, 100, 150);
    this.camera.lookAt(0, 0, 0);

    // Renderer setup
    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    this.renderer.setSize(this.container.clientWidth, this.container.clientHeight);
    this.renderer.setPixelRatio(window.devicePixelRatio);
    this.renderer.shadowMap.enabled = true;
    this.container.appendChild(this.renderer.domElement);

    // Lighting
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.5);
    this.scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
    directionalLight.position.set(100, 100, 100);
    directionalLight.castShadow = true;
    this.scene.add(directionalLight);

    // Create planet
    this.createPlanet();

    // Create falling object
    this.createFallingObject();

    // Add grid for reference
    this.addGrid();

    // Handle window resize
    this.onWindowResize = () => this.handleWindowResize();
    window.addEventListener('resize', this.onWindowResize);

    // Start animation loop
    this.animate();
  }

  createPlanet() {
    const radius = this.params.planetRadius / 100; // Scale down for visualization
    const geometry = new THREE.SphereGeometry(radius, 64, 64);
    const material = new THREE.MeshPhongMaterial({
      color: 0x3b82f6,
      emissive: 0x1e40af
    });
    this.planet = new THREE.Mesh(geometry, material);
    this.planet.castShadow = true;
    this.planet.receiveShadow = true;
    this.scene.add(this.planet);

    // Add planet label
    this.addLabel('Планета', 0, radius + 20, 0);
  }

  createFallingObject() {
    const geometry = new THREE.SphereGeometry(5, 32, 32);
    const material = new THREE.MeshPhongMaterial({
      color: 0xef4444,
      emissive: 0x991b1b
    });
    this.fallingObject = new THREE.Mesh(geometry, material);
    this.fallingObject.castShadow = true;
    this.fallingObject.receiveShadow = true;
    
    const planetRadius = this.params.planetRadius / 100;
    const height = Math.min(this.params.height / 100, 200);
    this.fallingObject.position.y = planetRadius + height;
    
    this.scene.add(this.fallingObject);
  }

  addGrid() {
    const gridHelper = new THREE.GridHelper(400, 40, 0xcccccc, 0xeeeeee);
    gridHelper.position.y = -this.params.planetRadius / 100 - 50;
    this.scene.add(gridHelper);
  }

  addLabel(text, x, y, z) {
    // Simple text label using canvas
    const canvas = document.createElement('canvas');
    canvas.width = 256;
    canvas.height = 64;
    const ctx = canvas.getContext('2d');
    ctx.fillStyle = '#1e40af';
    ctx.font = 'bold 32px Arial';
    ctx.textAlign = 'center';
    ctx.fillText(text, 128, 40);
    
    const texture = new THREE.CanvasTexture(canvas);
    const spriteMaterial = new THREE.SpriteMaterial({ map: texture });
    const sprite = new THREE.Sprite(spriteMaterial);
    sprite.position.set(x, y, z);
    sprite.scale.set(30, 8, 1);
    this.scene.add(sprite);
  }

  updateParameters(params) {
    this.params = params;
    
    // Update planet size
    if (this.planet) {
      this.scene.remove(this.planet);
      this.createPlanet();
    }
    
    // Reset falling object position
    if (this.fallingObject) {
      const planetRadius = this.params.planetRadius / 100;
      const height = Math.min(this.params.height / 100, 200);
      this.fallingObject.position.y = planetRadius + height;
    }
    
    this.elapsed = 0;
  }

  start() {
    this.isRunning = true;
    this.elapsed = 0;
  }

  stop() {
    this.isRunning = false;
  }

  animate() {
    this.animationId = requestAnimationFrame(() => this.animate());

    if (this.isRunning) {
      this.elapsed += 0.016; // Approximately 60 FPS

      // Physics calculation
      const G = 6.674e-11;
      const planetMass = this.params.planetMass * 1e24;
      const planetRadiusM = this.params.planetRadius * 1000;
      const acceleration = (G * planetMass) / (planetRadiusM * planetRadiusM);
      
      // Velocity: v = g*t
      const velocity = acceleration * this.elapsed;
      
      // Position: y = y0 - 0.5*g*t^2
      const displacement = 0.5 * acceleration * this.elapsed * this.elapsed;
      const initialHeight = Math.min(this.params.height / 100, 200);
      const planetRadius = this.params.planetRadius / 100;
      
      const newY = planetRadius + initialHeight - displacement / 100;
      
      // Stop if object hits planet
      if (newY <= planetRadius + 5) {
        this.stop();
        this.fallingObject.position.y = planetRadius + 5;
      } else {
        this.fallingObject.position.y = newY;
      }
    }

    this.renderer.render(this.scene, this.camera);
  }

  handleWindowResize() {
    const width = this.container.clientWidth;
    const height = this.container.clientHeight;
    
    this.camera.aspect = width / height;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(width, height);
  }

  dispose() {
    window.removeEventListener('resize', this.onWindowResize);
    cancelAnimationFrame(this.animationId);
    
    this.renderer.dispose();
    this.container.removeChild(this.renderer.domElement);
    
    // Clean up geometries and materials
    this.scene.traverse((obj) => {
      if (obj.geometry) obj.geometry.dispose();
      if (obj.material) obj.material.dispose();
    });
  }
}
