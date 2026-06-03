import * as THREE from 'three';

export default class GeometryShapes {
  constructor(container, params) {
    this.container = container;
    this.params = params;
    this.animationId = null;
    this.shape = null;

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
    this.camera.position.set(50, 50, 50);
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

    // Add grid
    const gridHelper = new THREE.GridHelper(100, 20, 0xcccccc, 0xeeeeee);
    this.scene.add(gridHelper);

    // Create initial shape
    this.createShape();

    // Handle mouse events for rotation
    this.setupMouseControls();

    // Handle window resize
    this.onWindowResize = () => this.handleWindowResize();
    window.addEventListener('resize', this.onWindowResize);

    // Start animation loop
    this.animate();
  }

  createShape() {
    if (this.shape) {
      this.scene.remove(this.shape);
    }

    let geometry;
    const material = new THREE.MeshStandardMaterial({
      color: 0x8b5cf6,
      metalness: 0.3,
      roughness: 0.6
    });

    const type = this.params.type || 'cube';
    const size = this.params.size || 20;

    switch (type) {
      case 'cube':
        geometry = new THREE.BoxGeometry(size, size, size);
        break;
      case 'sphere':
        geometry = new THREE.SphereGeometry(size / 2, 64, 64);
        break;
      case 'cylinder':
        geometry = new THREE.CylinderGeometry(size / 2, size / 2, size, 32);
        break;
      case 'cone':
        geometry = new THREE.ConeGeometry(size / 2, size, 32);
        break;
      case 'torus':
        geometry = new THREE.TorusGeometry(size / 2, size / 4, 16, 100);
        break;
      case 'tetrahedron':
        geometry = new THREE.TetrahedronGeometry(size / 2);
        break;
      case 'octahedron':
        geometry = new THREE.OctahedronGeometry(size / 2);
        break;
      case 'dodecahedron':
        geometry = new THREE.DodecahedronGeometry(size / 2);
        break;
      case 'icosahedron':
        geometry = new THREE.IcosahedronGeometry(size / 2);
        break;
      default:
        geometry = new THREE.BoxGeometry(size, size, size);
    }

    this.shape = new THREE.Mesh(geometry, material);
    this.shape.castShadow = true;
    this.shape.receiveShadow = true;
    this.scene.add(this.shape);

    // Display shape properties
    this.displayProperties(geometry);
  }

  displayProperties(geometry) {
    geometry.computeBoundingBox();
    const bbox = geometry.boundingBox;
    const size = bbox.max.clone().sub(bbox.min);
    
    // Update params with calculated values
    this.params.volume = size.x * size.y * size.z;
    this.params.surfaceArea = this.calculateSurfaceArea(geometry);
  }

  calculateSurfaceArea(geometry) {
    if (!geometry.attributes.position) return 0;

    let area = 0;
    const positions = geometry.attributes.position;
    
    for (let i = 0; i < positions.count; i += 3) {
      const v1 = new THREE.Vector3(positions.getX(i), positions.getY(i), positions.getZ(i));
      const v2 = new THREE.Vector3(positions.getX(i + 1), positions.getY(i + 1), positions.getZ(i + 1));
      const v3 = new THREE.Vector3(positions.getX(i + 2), positions.getY(i + 2), positions.getZ(i + 2));
      
      const edge1 = v2.clone().sub(v1);
      const edge2 = v3.clone().sub(v1);
      const cross = edge1.cross(edge2);
      area += cross.length() / 2;
    }

    return area;
  }

  setupMouseControls() {
    let isDragging = false;
    let previousMousePosition = { x: 0, y: 0 };

    this.container.addEventListener('mousedown', (e) => {
      isDragging = true;
      previousMousePosition = { x: e.clientX, y: e.clientY };
    });

    this.container.addEventListener('mousemove', (e) => {
      if (isDragging && this.shape) {
        const deltaX = e.clientX - previousMousePosition.x;
        const deltaY = e.clientY - previousMousePosition.y;

        this.shape.rotation.y += deltaX * 0.01;
        this.shape.rotation.x += deltaY * 0.01;

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
      this.camera.position.copy(direction.multiplyScalar(Math.max(30, newDistance)));
      this.camera.lookAt(0, 0, 0);
    });
  }

  updateParameters(params) {
    this.params = params;
    this.createShape();
  }

  animate() {
    this.animationId = requestAnimationFrame(() => this.animate());
    
    // Slow auto-rotation if not being dragged
    if (this.shape && !this.isDragging) {
      this.shape.rotation.x += 0.002;
      this.shape.rotation.y += 0.003;
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
