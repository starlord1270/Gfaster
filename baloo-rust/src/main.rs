mod db;
mod indexer;

use clap::{Parser, Subcommand};
use db::IndexDatabase;
use indexer::{scan_and_index, IndexerOptions};
use std::path::PathBuf;

#[derive(Parser)]
#[command(name = "gfaster-rust")]
#[command(about = "GFaster - Indexador de archivos ultrarrápido en Rust para Arch Linux y KDE", long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Commands,

    #[arg(short, long, default_value = "~/.local/share/gfaster-db")]
    db_path: String,
}

#[derive(Subcommand)]
enum Commands {
    /// Escanear e indexar un directorio
    Index {
        #[arg(default_value = ".")]
        path: PathBuf,
    },
    /// Buscar archivos instantáneamente por nombre o extensión
    Search {
        query: String,
        #[arg(short, long, default_value_t = 50)]
        limit: usize,
    },
    /// Mostrar estadísticas de la base de datos de indexación
    Stats,
    /// Compactar la base de datos sled
    Compact,
}

fn resolve_db_path(raw: &str) -> PathBuf {
    if raw.starts_with("~/") {
        if let Ok(home) = std::env::var("HOME") {
            return PathBuf::from(home).join(&raw[2..]);
        }
    }
    PathBuf::from(raw)
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let cli = Cli::parse();
    let db_path = resolve_db_path(&cli.db_path);

    let db = IndexDatabase::open(&db_path)?;

    match cli.command {
        Commands::Index { path } => {
            println!("🚀 Iniciando indexado ultrarrápido en Rust en: {:?}", path);
            let options = IndexerOptions::default();
            let stats = scan_and_index(&path, &db, &options)?;

            println!("✅ Indexado completado:");
            println!("   - Archivos procesados: {}", stats.files_indexed);
            println!("   - Tamaño total: {} MB", stats.bytes_indexed / 1_048_576);
            println!("   - Tiempo transcurrido: {} ms", stats.elapsed_ms);
        }
        Commands::Search { query, limit } => {
            let start = std::time::Instant::now();
            let results = db.search_filename(&query);
            let elapsed = start.elapsed().as_micros();

            println!("🔎 Resultados de búsqueda para '{}' ({} µs):", query, elapsed);
            for (i, item) in results.iter().take(limit).enumerate() {
                println!(
                    " {:3}. [{}] {} ({:.2} KB)",
                    i + 1,
                    item.mime_type,
                    item.path,
                    item.size_bytes as f64 / 1024.0
                );
            }
            println!("Total mostrados: {}", results.len().min(limit));
        }
        Commands::Stats => {
            println!("📊 Estadísticas del Indexador Rust:");
            println!("   - Entradas totales en DB: {}", db.count());
            println!("   - Tamaño de base de datos: {:.2} MB", db.size_on_disk() as f64 / 1_048_576.0);
        }
        Commands::Compact => {
            println!("🧹 Compactando la base de datos...");
            db.compact()?;
            println!("✨ Base de datos compactada correctamente.");
        }
    }

    Ok(())
}
