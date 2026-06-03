import * as THREE from 'three';

export default class ProjectileSimulation {
  constructor(container, params) {
    this.container = container;
    this.params = params;
    this.isRunning = false;
    this.elapsed = 0;
    this.animationId = null;
    this.trajectoryPoints = [];

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
    this.camera.position.set(50, 40, 100);
    this.camera.lookAt(0, 0, 0);

    // Renderer setup
    this.renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
    this.renderer.setSize(this.container.clientWidth, this.container.clientHeight);
    this.renderer.setPixelRatio(window.devicePixelRatio);
    this.renderer.shadowMap.enabled = true;
    this.container.appendChild(this.renderer.domElement);

    // Lighting
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
    this.scene.add(ambientLight);

    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
    directionalLight.position.set(100, 100, 100);
    directionalLight.castShadow = true;
    this.scene.add(directionalLight);

    // Create ground
    this.createGround();

    // Create projectile
    this.createProjectile();

    // Add grid for reference
    this.addGrid();

    // Handle window resize
    this.onWindowResize = () => this.handleWindowResize();
    window.addEventListener('resize', this.onWindowResize);

    // Start animation loop
    this.animate();
  }

  createGround() {
    const groundGeometry = new THREE.PlaneGeometry(500, 500);
    const groundMaterial = new THREE.MeshStandardMaterial({ 
      color: 0x7c6d5f,
      metalness: 0.1,
      roughness: 0.8
    });
    const ground = new THREE.Mesh(groundGeometry, groundMaterial);
    ground.rotation.x = -Math.PI / 2;
    ground.receiveShadow = true;
    this.scene.add(ground);
  }

  createProjectile() {
    const projectileGeometry = new THREE.SphereGeometry(2, 32, 32);
    const projectileMaterial = new THREE.MeshPhongMaterial({
      color: 0xfbbf24,
      emissive: 0xd97706,
      shininess: 100
    });
    this.projectile = new THREE.Mesh(projectileGeometry, projectileMaterial);
    this.projectile.castShadow = true;
    this.projectile.receiveShadow = true;
    this.projectile.position.y = 2;
    this.scene.add(this.projectile);

    // Create trajectory line
    this.trajectoryLine = new THREE.Line(
      new THREE.BufferGeometry(),
      new THREE.LineBasicMaterial({ color: 0x3b82f6, linewidth: 2 })
    );
    this.scene.add(this.trajectoryLine);
  }

  addGrid() {
    const gridHelper = new THREE.GridHelper(400, 40, 0xcccccc, 0xeeeeee);
    this.scene.add(gridHelper);
  }

  updateParameters(params) {
    this.params = params;
    this.elapsed = 0;
    this.trajectoryPoints = [];
    
    // Reset projectile position
    this.projectile.position.set(0, 2, 0);
    
    // Clear trajectory
    this.trajectoryLine.geometry.dispose();
    this.trajectoryLine.geometry = new THREE.BufferGeometry();
  }

  start() {
    this.isRunning = true;
    this.elapsed = 0;
    this.trajectoryPoints = [];
  }

  stop() {
    this.isRunning = false;
  }

  animate() {
    this.animationId = requestAnimationFrame(() => this.animate());

    if (this.isRunning) {
      this.elapsed += 0.016; // Approximately 60 FPS

      // Physics calculation
      const g = 9.81; // Gravity (m/s²)
      const v0 = this.params.velocity; // Initial velocity (m/s)
      const angle = (this.params.angle * Math.PI) / 180; // Angle in radians
      
      // Air resistance coefficient
      const airResistance = this.params.airResistance ? 0.01 : 0;
      
      // Velocity components
      const vx0 = v0 * Math.cos(angle);
      const vy0 = v0 * Math.sin(angle);
      
      // Position with air resistance (simplified)
      const decay = Math.pow(Math.E, -airResistance * this.elapsed);
      const x = (vx0 / airResistance) * (1 - decay);
      const y = (vy0 / airResistance) * (1 - decay) - 0.5 * g * this.elapsed * this.elapsed;
      
      // Stop if projectile hits ground
      if (y <= 0) {
        this.stop();
        this.projectile.position.set(x, 0, 0);
      } else {
        this.projectile.position.set(x, Math.max(0, y), 0);
        
        // Add trajectory point
        this.trajectoryPoints.push(new THREE.Vector3(x, Math.max(0, y), 0));
        
        // Update trajectory line
        this.trajectoryLine.geometry.dispose();
        this.trajectoryLine.geometry = new THREE.BufferGeometry();
        this.trajectoryLine.geometry.setFromPoints(this.trajectoryPoints);
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
