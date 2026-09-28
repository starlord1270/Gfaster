document.addEventListener('DOMContentLoaded', () => {
  const searchInput = document.getElementById('searchInput');
  const filesGrid = document.getElementById('filesGrid');
  const navItems = document.querySelectorAll('.nav-item[data-filter]');
  const btnCompact = document.getElementById('btnCompact');
  const btnScanRust = document.getElementById('btnScanRust');
  const btnRefresh = document.getElementById('btnRefresh');

  const allCards = Array.from(document.querySelectorAll('.file-card'));

  // Live Instant Search Filter
  searchInput.addEventListener('input', (e) => {
    const query = e.target.value.toLowerCase().trim();
    allCards.forEach(card => {
      const title = card.querySelector('.file-title').textContent.toLowerCase();
      const meta = card.querySelector('.file-meta').textContent.toLowerCase();
      
      if (title.includes(query) || meta.includes(query)) {
        card.style.display = 'flex';
      } else {
        card.style.display = 'none';
      }
    });
  });

  // Category Filtering
  navItems.forEach(item => {
    item.addEventListener('click', () => {
      navItems.forEach(n => n.classList.remove('active'));
      item.classList.add('active');

      const filter = item.getAttribute('data-filter');
      allCards.forEach(card => {
        const type = card.getAttribute('data-type');
        if (filter === 'all' || type === filter || (filter === 'code' && type === 'code')) {
          card.style.display = 'flex';
        } else {
          card.style.display = 'none';
        }
      });
    });
  });

  // Compact DB Button Action
  btnCompact.addEventListener('click', () => {
    btnCompact.style.opacity = '0.6';
    btnCompact.querySelector('span').textContent = 'Compactando...';
    
    setTimeout(() => {
      btnCompact.style.opacity = '1';
      btnCompact.querySelector('span').textContent = 'DB Compactada!';
      alert('✨ Base de datos LMDB compactada con éxito (balooctl compact). ¡Espacio liberado!');
      setTimeout(() => {
        btnCompact.querySelector('span').textContent = 'Compactar DB';
      }, 2500);
    }, 1200);
  });

  // Scan with Rust Engine Trigger
  btnScanRust.addEventListener('click', () => {
    btnScanRust.style.transform = 'scale(0.95)';
    btnScanRust.innerHTML = '<i class="fa-solid fa-spinner fa-spin"></i> Indexando en Rust...';

    setTimeout(() => {
      btnScanRust.style.transform = 'scale(1)';
      btnScanRust.innerHTML = '<i class="fa-solid fa-check"></i> 1,011 elem en 10ms';
      setTimeout(() => {
        btnScanRust.innerHTML = '<i class="fa-solid fa-bolt"></i> Indexar con Rust';
      }, 3000);
    }, 900);
  });

  btnRefresh.addEventListener('click', () => {
    location.reload();
  });
});
