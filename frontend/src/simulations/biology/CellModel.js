import * as THREE from 'three';

export default class CellModel {
  constructor(container, params) {
    this.container = container;
    this.params = params;
    this.animationId = null;
    this.organelles = [];

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
    this.camera.position.set(0, 0, 80);
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
    directionalLight.position.set(50, 50, 50);
    directionalLight.castShadow = true;
    this.scene.add(directionalLight);

    // Create cell
    this.createCell();

    // Handle mouse events for rotation
    this.setupMouseControls();

    // Handle window resize
    this.onWindowResize = () => this.handleWindowResize();
    window.addEventListener('resize', this.onWindowResize);

    // Start animation loop
    this.animate();
  }

  createCell() {
    // Clear existing organelles
    this.organelles.forEach(org => this.scene.remove(org));
    this.organelles = [];

    // Cell membrane (outer boundary)
    const membraneGeometry = new THREE.SphereGeometry(35, 64, 64);
    const membraneMaterial = new THREE.MeshStandardMaterial({
      color: 0xccff99,
      metalness: 0.2,
      roughness: 0.4,
      side: THREE.BackSide,
      transparent: true,
      opacity: 0.3
    });
    const membrane = new THREE.Mesh(membraneGeometry, membraneMaterial);
    this.scene.add(membrane);
    this.organelles.push(membrane);

    // Nucleus (center)
    const nucleusGeometry = new THREE.SphereGeometry(15, 32, 32);
    const nucleusMaterial = new THREE.MeshStandardMaterial({
      color: 0xff6b9d,
      metalness: 0.3,
      roughness: 0.5
    });
    const nucleus = new THREE.Mesh(nucleusGeometry, nucleusMaterial);
    nucleus.position.set(0, 0, 0);
    this.scene.add(nucleus);
    this.organelles.push(nucleus);

    // Nucleolus (inside nucleus)
    const nucleolusGeometry = new THREE.SphereGeometry(6, 24, 24);
    const nucleolusMaterial = new THREE.MeshStandardMaterial({
      color: 0xff1493,
      metalness: 0.4,
      roughness: 0.3
    });
    const nucleolus = new THREE.Mesh(nucleolusGeometry, nucleolusMaterial);
    nucleolus.position.set(0, 0, 0);
    this.scene.add(nucleolus);
    this.organelles.push(nucleolus);

    // Mitochondria
    for (let i = 0; i < 3; i++) {
      const angle = (i / 3) * Math.PI * 2;
      const mitoGeometry = new THREE.CylinderGeometry(3, 3, 12, 16);
      const mitoMaterial = new THREE.MeshStandardMaterial({
        color: 0xffb347,
        metalness: 0.3,
        roughness: 0.5
      });
      const mitochondrion = new THREE.Mesh(mitoGeometry, mitoMaterial);
      mitochondrion.position.set(
        Math.cos(angle) * 22,
        Math.sin(angle) * 8,
        0
      );
      mitochondrion.rotation.z = angle;
      this.scene.add(mitochondrion);
      this.organelles.push(mitochondrion);
    }

    // Ribosomes
    for (let i = 0; i < 8; i++) {
      const angle = (i / 8) * Math.PI * 2;
      const riboGeometry = new THREE.SphereGeometry(2, 16, 16);
      const riboMaterial = new THREE.MeshStandardMaterial({
        color: 0xff9999,
        metalness: 0.5,
        roughness: 0.3
      });
      const ribosome = new THREE.Mesh(riboGeometry, riboMaterial);
      ribosome.position.set(
        Math.cos(angle) * 28,
        Math.sin(angle) * 22,
        Math.cos(angle + Math.PI) * 15
      );
      this.scene.add(ribosome);
      this.organelles.push(ribosome);
    }

    // Golgi apparatus
    for (let i = 0; i < 4; i++) {
      const stackGeometry = new THREE.CylinderGeometry(4, 4, 2, 16);
      const stackMaterial = new THREE.MeshStandardMaterial({
        color: 0xccff00,
        metalness: 0.3,
        roughness: 0.5
      });
      const stack = new THREE.Mesh(stackGeometry, stackMaterial);
      stack.position.set(0, -12 - i * 3, 20);
      this.scene.add(stack);
      this.organelles.push(stack);
    }

    // Vacuole
    const vacuoleGeometry = new THREE.SphereGeometry(8, 24, 24);
    const vacuoleMaterial = new THREE.MeshStandardMaterial({
      color: 0x87ceeb,
      metalness: 0.2,
      roughness: 0.6,
      transparent: true,
      opacity: 0.5
    });
    const vacuole = new THREE.Mesh(vacuoleGeometry, vacuoleMaterial);
    vacuole.position.set(-18, -15, 0);
    this.scene.add(vacuole);
    this.organelles.push(vacuole);

    // Endoplasmic reticulum (simplified as flat planes)
    for (let i = 0; i < 3; i++) {
      const erGeometry = new THREE.PlaneGeometry(15, 2);
      const erMaterial = new THREE.MeshStandardMaterial({
        color: 0x90ee90,
        metalness: 0.2,
        roughness: 0.5
      });
      const er = new THREE.Mesh(erGeometry, erMaterial);
      er.position.set(8 - i * 8, 8 + i * 6, 15 - i * 10);
      er.rotation.x = Math.PI / 4;
      this.scene.add(er);
      this.organelles.push(er);
    }
  }

  setupMouseControls() {
    let isDragging = false;
    let previousMousePosition = { x: 0, y: 0 };

    this.container.addEventListener('mousedown', (e) => {
      isDragging = true;
      previousMousePosition = { x: e.clientX, y: e.clientY };
    });

    this.container.addEventListener('mousemove', (e) => {
      if (isDragging) {
        const deltaX = e.clientX - previousMousePosition.x;
        const deltaY = e.clientY - previousMousePosition.y;

        this.organelles.forEach(org => {
          org.rotation.y += deltaX * 0.01;
          org.rotation.x += deltaY * 0.01;
        });

        previousMousePosition = { x: e.clientX, y: e.clientY };
      }
    });

    this.container.addEventListener('mouseup', () => {
      isDragging = false;
    });

    this.container.addEventListener('wheel', (e) => {
      e.preventDefault();
      const direction = this.camera.position.clone().normalize();
      const distance = this.camera.position.length();
      const newDistance = distance + (e.deltaY > 0 ? 5 : -5);
      this.camera.position.copy(direction.multiplyScalar(Math.max(40, newDistance)));
      this.camera.lookAt(0, 0, 0);
    });
  }

  updateParameters(params) {
    this.params = params;
  }

  animate() {
    this.animationId = requestAnimationFrame(() => this.animate());
    
    // Auto-rotate organelles slowly
    this.organelles.forEach((org, index) => {
      if (index > 0) {
        org.rotation.x += 0.0005 * (index % 2 === 0 ? 1 : -1);
        org.rotation.y += 0.0008 * (index % 3 === 0 ? 1 : -1);
      }
    });

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
