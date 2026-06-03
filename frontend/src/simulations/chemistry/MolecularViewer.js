import * as THREE from 'three';

// Simple molecular structures
const MOLECULES = {
  water: {
    atoms: [
      { type: 'O', position: [0, 0, 0], color: 0xff0000, radius: 7 },
      { type: 'H', position: [-5, 8, 0], color: 0xffffff, radius: 5 },
      { type: 'H', position: [5, 8, 0], color: 0xffffff, radius: 5 }
    ],
    bonds: [
      [0, 1],
      [0, 2]
    ]
  },
  methane: {
    atoms: [
      { type: 'C', position: [0, 0, 0], color: 0x333333, radius: 7 },
      { type: 'H', position: [-8, 8, 0], color: 0xffffff, radius: 5 },
      { type: 'H', position: [8, 8, 0], color: 0xffffff, radius: 5 },
      { type: 'H', position: [0, -8, 8], color: 0xffffff, radius: 5 },
      { type: 'H', position: [0, 8, -8], color: 0xffffff, radius: 5 }
    ],
    bonds: [
      [0, 1],
      [0, 2],
      [0, 3],
      [0, 4]
    ]
  },
  carbonDioxide: {
    atoms: [
      { type: 'C', position: [0, 0, 0], color: 0x333333, radius: 7 },
      { type: 'O', position: [-12, 0, 0], color: 0xff0000, radius: 7 },
      { type: 'O', position: [12, 0, 0], color: 0xff0000, radius: 7 }
    ],
    bonds: [
      [0, 1],
      [0, 2]
    ]
  },
  ammonia: {
    atoms: [
      { type: 'N', position: [0, 0, 0], color: 0x3050f8, radius: 7 },
      { type: 'H', position: [-8, 8, 0], color: 0xffffff, radius: 5 },
      { type: 'H', position: [8, 8, 0], color: 0xffffff, radius: 5 },
      { type: 'H', position: [0, -8, 8], color: 0xffffff, radius: 5 }
    ],
    bonds: [
      [0, 1],
      [0, 2],
      [0, 3]
    ]
  }
};

export default class MolecularViewer {
  constructor(container, params) {
    this.container = container;
    this.params = params;
    this.animationId = null;
    this.atoms = [];
    this.bonds = [];

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
    this.camera.position.set(0, 0, 60);
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
    directionalLight.castShadow = true;
    this.scene.add(directionalLight);

    // Create molecule
    this.createMolecule();

    // Handle mouse events for rotation
    this.setupMouseControls();

    // Handle window resize
    this.onWindowResize = () => this.handleWindowResize();
    window.addEventListener('resize', this.onWindowResize);

    // Start animation loop
    this.animate();
  }

  createMolecule() {
    // Clear existing
    this.atoms.forEach(atom => this.scene.remove(atom));
    this.bonds.forEach(bond => this.scene.remove(bond));
    this.atoms = [];
    this.bonds = [];

    const moleculeType = this.params.type || 'water';
    const molecule = MOLECULES[moleculeType];

    if (!molecule) return;

    // Create atoms
    molecule.atoms.forEach((atomData, index) => {
      const geometry = new THREE.SphereGeometry(atomData.radius / 10, 32, 32);
      const material = new THREE.MeshPhongMaterial({
        color: atomData.color,
        shininess: 100
      });
      const atom = new THREE.Mesh(geometry, material);
      atom.position.set(...atomData.position);
      atom.castShadow = true;
      atom.receiveShadow = true;
      this.scene.add(atom);
      this.atoms.push(atom);
    });

    // Create bonds
    molecule.bonds.forEach((bond) => {
      const [i, j] = bond;
      const startPos = new THREE.Vector3(...molecule.atoms[i].position);
      const endPos = new THREE.Vector3(...molecule.atoms[j].position);
      const direction = endPos.clone().sub(startPos);
      const length = direction.length();

      const bondGeometry = new THREE.CylinderGeometry(0.5, 0.5, length, 16);
      const bondMaterial = new THREE.MeshStandardMaterial({
        color: 0xcccccc,
        metalness: 0.3,
        roughness: 0.6
      });
      const bondMesh = new THREE.Mesh(bondGeometry, bondMaterial);

      const midpoint = startPos.clone().add(endPos).multiplyScalar(0.5);
      bondMesh.position.copy(midpoint);
      bondMesh.lookAt(endPos);
      bondMesh.rotateX(Math.PI / 2);

      this.scene.add(bondMesh);
      this.bonds.push(bondMesh);
    });
  }

  setupMouseControls() {
    let isDragging = false;
    let previousMousePosition = { x: 0, y: 0 };
    this.moleculeGroup = new THREE.Group();
    
    this.atoms.forEach(atom => this.moleculeGroup.add(atom));
    this.bonds.forEach(bond => this.moleculeGroup.add(bond));
    
    if (this.moleculeGroup.children.length > 0) {
      this.scene.add(this.moleculeGroup);
    }

    this.container.addEventListener('mousedown', (e) => {
      isDragging = true;
      previousMousePosition = { x: e.clientX, y: e.clientY };
    });

    this.container.addEventListener('mousemove', (e) => {
      if (isDragging && this.moleculeGroup) {
        const deltaX = e.clientX - previousMousePosition.x;
        const deltaY = e.clientY - previousMousePosition.y;

        this.moleculeGroup.rotation.y += deltaX * 0.01;
        this.moleculeGroup.rotation.x += deltaY * 0.01;

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
    this.createMolecule();
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
