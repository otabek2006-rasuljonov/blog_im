import * as THREE from 'three';

export default class FunctionGraphing {
  constructor(container, params) {
    this.container = container;
    this.params = params;
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
    this.container.appendChild(this.renderer.domElement);

    // Lighting
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.8);
    this.scene.add(ambientLight);

    // Create axes
    this.createAxes();

    // Create function graph
    this.createGraph();

    // Handle mouse events for rotation
    this.setupMouseControls();

    // Handle window resize
    this.onWindowResize = () => this.handleWindowResize();
    window.addEventListener('resize', this.onWindowResize);

    // Start animation loop
    this.animate();
  }

  createAxes() {
    const axisLength = 50;
    const axisRadius = 0.5;

    // X axis (red)
    const xGeometry = new THREE.CylinderGeometry(axisRadius, axisRadius, axisLength, 32);
    const xMaterial = new THREE.MeshStandardMaterial({ color: 0xff0000 });
    const xAxis = new THREE.Mesh(xGeometry, xMaterial);
    xAxis.rotation.z = Math.PI / 2;
    xAxis.position.x = axisLength / 2;
    this.scene.add(xAxis);

    // Y axis (green)
    const yGeometry = new THREE.CylinderGeometry(axisRadius, axisRadius, axisLength, 32);
    const yMaterial = new THREE.MeshStandardMaterial({ color: 0x00aa00 });
    const yAxis = new THREE.Mesh(yGeometry, yMaterial);
    yAxis.position.y = axisLength / 2;
    this.scene.add(yAxis);

    // Z axis (blue)
    const zGeometry = new THREE.CylinderGeometry(axisRadius, axisRadius, axisLength, 32);
    const zMaterial = new THREE.MeshStandardMaterial({ color: 0x0000ff });
    const zAxis = new THREE.Mesh(zGeometry, zMaterial);
    zAxis.rotation.x = Math.PI / 2;
    zAxis.position.z = axisLength / 2;
    this.scene.add(zAxis);

    // Grid
    const gridHelper = new THREE.GridHelper(100, 20, 0xcccccc, 0xeeeeee);
    gridHelper.position.y = -30;
    this.scene.add(gridHelper);
  }

  createGraph() {
    const xMin = -20;
    const xMax = 20;
    const step = 0.5;
    const points = [];

    for (let x = xMin; x <= xMax; x += step) {
      const y = this.evaluateFunction(x);
      points.push(new THREE.Vector3(x, y, 0));
    }

    const geometry = new THREE.BufferGeometry();
    geometry.setFromPoints(points);
    const material = new THREE.LineBasicMaterial({ color: 0x3b82f6, linewidth: 2 });
    
    this.graphLine = new THREE.Line(geometry, material);
    this.scene.add(this.graphLine);

    // Add points
    const pointsGeometry = new THREE.BufferGeometry();
    pointsGeometry.setFromPoints(points.filter((_, i) => i % 4 === 0));
    const pointsMaterial = new THREE.PointsMaterial({ 
      color: 0x3b82f6, 
      size: 1.5,
      sizeAttenuation: true
    });
    this.points = new THREE.Points(pointsGeometry, pointsMaterial);
    this.scene.add(this.points);
  }

  evaluateFunction(x) {
    const a = this.params.amplitude || 1;
    const b = this.params.frequency || 1;
    const c = this.params.phase || 0;
    const type = this.params.type || 'sine';

    switch (type) {
      case 'sine':
        return a * Math.sin(b * x + c);
      case 'cosine':
        return a * Math.cos(b * x + c);
      case 'tangent':
        return a * Math.tan(b * x + c);
      case 'quadratic':
        return a * x * x + b * x + c;
      case 'cubic':
        return a * x * x * x + b * x * x + c * x;
      case 'exponential':
        return a * Math.pow(2, b * x);
      case 'logarithm':
        return a * Math.log(Math.abs(b * x + 1));
      default:
        return a * Math.sin(b * x + c);
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

        this.camera.position.applyAxisAngle(
          new THREE.Vector3(0, 1, 0),
          deltaX * 0.01
        );
        this.camera.position.applyAxisAngle(
          new THREE.Vector3(1, 0, 0),
          deltaY * 0.01
        );
        this.camera.lookAt(0, 0, 0);

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
      this.camera.position.copy(direction.multiplyScalar(Math.max(20, newDistance)));
      this.camera.lookAt(0, 0, 0);
    });
  }

  updateParameters(params) {
    this.params = params;
    
    // Recreate graph
    this.scene.remove(this.graphLine);
    this.scene.remove(this.points);
    this.createGraph();
  }

  animate() {
    this.animationId = requestAnimationFrame(() => this.animate());
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
