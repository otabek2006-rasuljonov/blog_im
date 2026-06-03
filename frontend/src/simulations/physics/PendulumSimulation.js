import * as THREE from 'three';

export default class PendulumSimulation {
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
    this.camera.position.set(0, 0, 100);
    this.camera.lookAt(0, 0, 0);

    // Renderer setup
    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    this.renderer.setSize(this.container.clientWidth, this.container.clientHeight);
    this.renderer.setPixelRatio(window.devicePixelRatio);
    this.renderer.shadowMap.enabled = true;
    this.container.appendChild(this.renderer.domElement);

    // Lighting
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.7);
    this.scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
    directionalLight.position.set(50, 50, 50);
    this.scene.add(directionalLight);

    // Create pendulum
    this.createPendulum();

    // Add reference lines and grid
    this.addReferenceElements();

    // Handle window resize
    this.onWindowResize = () => this.handleWindowResize();
    window.addEventListener('resize', this.onWindowResize);

    // Start animation loop
    this.animate();
  }

  createPendulum() {
    // Pivot point
    const pivotGeometry = new THREE.SphereGeometry(3, 32, 32);
    const pivotMaterial = new THREE.MeshPhongMaterial({ color: 0x000000 });
    this.pivot = new THREE.Mesh(pivotGeometry, pivotMaterial);
    this.pivot.position.y = 50;
    this.scene.add(this.pivot);

    // Rod
    const rodGeometry = new THREE.CylinderGeometry(0.5, 0.5, this.params.length * 10, 32);
    const rodMaterial = new THREE.MeshPhongMaterial({ color: 0x4f46e5 });
    this.rod = new THREE.Mesh(rodGeometry, rodMaterial);
    this.rod.position.y = 50 - (this.params.length * 10) / 2;
    this.scene.add(this.rod);

    // Bob (mass)
    const bobGeometry = new THREE.SphereGeometry(8, 32, 32);
    const bobMaterial = new THREE.MeshPhongMaterial({
      color: 0xef4444,
      emissive: 0x991b1b
    });
    this.bob = new THREE.Mesh(bobGeometry, bobMaterial);
    this.bob.castShadow = true;
    this.bob.receiveShadow = true;
    this.scene.add(this.bob);

    // Rod container for rotation
    this.rodGroup = new THREE.Group();
    this.rodGroup.add(this.rod);
    this.rodGroup.add(this.bob);
    this.rod.position.y = -(this.params.length * 10) / 2;
    this.bob.position.y = -(this.params.length * 10);
    
    this.scene.remove(this.rod);
    this.scene.remove(this.bob);
    this.scene.add(this.rodGroup);
    this.rodGroup.position.y = 50;
  }

  addReferenceElements() {
    // Vertical reference line
    const lineGeometry = new THREE.BufferGeometry();
    const linePoints = [
      new THREE.Vector3(0, 100, 0),
      new THREE.Vector3(0, -100, 0)
    ];
    lineGeometry.setFromPoints(linePoints);
    const lineMaterial = new THREE.LineBasicMaterial({ color: 0xcccccc, linewidth: 2 });
    const line = new THREE.Line(lineGeometry, lineMaterial);
    this.scene.add(line);

    // Arc to show amplitude
    this.drawArc();
  }

  drawArc() {
    const radius = this.params.length * 10;
    const angle = (this.params.angle * Math.PI) / 180;
    
    const points = [];
    for (let i = -angle; i <= angle; i += 0.1) {
      const x = radius * Math.sin(i);
      const y = -radius * Math.cos(i);
      points.push(new THREE.Vector3(x, y, 0));
    }

    const arcGeometry = new THREE.BufferGeometry();
    arcGeometry.setFromPoints(points);
    const arcMaterial = new THREE.LineBasicMaterial({ color: 0x9ca3af });
    const arc = new THREE.Line(arcGeometry, arcMaterial);
    this.rodGroup.add(arc);
  }

  updateParameters(params) {
    this.params = params;
    
    // Recreate pendulum with new parameters
    if (this.rodGroup) {
      this.scene.remove(this.rodGroup);
    }
    this.createPendulum();
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

      // Pendulum physics: simple harmonic motion
      // θ(t) = θ₀ * cos(√(g/L) * t)
      const g = this.params.gravity;
      const L = this.params.length;
      const omega = Math.sqrt(g / L); // Angular frequency
      const theta0 = (this.params.angle * Math.PI) / 180; // Initial angle in radians
      
      const theta = theta0 * Math.cos(omega * this.elapsed);
      
      // Rotate the rod group
      this.rodGroup.rotation.z = theta;
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
