use crate::db::{FileEntry, IndexDatabase};
use rayon::prelude::*;
use std::fs;
use std::path::Path;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::time::SystemTime;
use walkdir::WalkDir;

pub struct IndexerOptions {
    pub exclude_dirs: Vec<String>,
}

impl Default for IndexerOptions {
    fn default() -> Self {
        Self {
            exclude_dirs: vec![
                ".git".to_string(),
                "node_modules".to_string(),
                "target".to_string(),
                "dist".to_string(),
                "out".to_string(),
                ".cache".to_string(),
                ".next".to_string(),
                ".venv".to_string(),
                "venv".to_string(),
                "__pycache__".to_string(),
                ".cargo".to_string(),
                ".gradle".to_string(),
                "vendor".to_string(),
            ],
        }
    }
}

pub struct IndexerStats {
    pub files_indexed: usize,
    pub bytes_indexed: u64,
    pub elapsed_ms: u128,
}

pub fn scan_and_index<P: AsRef<Path>>(
    root: P,
    db: &IndexDatabase,
    options: &IndexerOptions,
) -> Result<IndexerStats, Box<dyn std::error::Error>> {
    let start_time = std::time::Instant::now();
    let root_path = root.as_ref();
    let exclude_dirs = &options.exclude_dirs;

    let entries: Vec<walkdir::DirEntry> = WalkDir::new(root_path)
        .into_iter()
        .filter_entry(|e| {
            if let Some(name) = e.file_name().to_str() {
                if e.file_type().is_dir() && exclude_dirs.iter().any(|ex| ex == name) {
                    return false;
                }
            }
            true
        })
        .filter_map(|e| e.ok())
        .collect();

    let count = AtomicUsize::new(0);
    let total_bytes = std::sync::atomic::AtomicU64::new(0);

    // Parallel processing with Rayon for max CPU throughput
    entries.par_iter().for_each(|entry| {
        let path = entry.path();
        let path_str = path.to_string_lossy().to_string();
        let filename = path
            .file_name()
            .map(|f| f.to_string_lossy().to_string())
            .unwrap_or_default();

        let extension = path
            .extension()
            .map(|ext| ext.to_string_lossy().to_string().to_lowercase())
            .unwrap_or_default();

        let metadata = match fs::metadata(path) {
            Ok(m) => m,
            Err(_) => return,
        };

        let is_dir = metadata.is_dir();
        let size_bytes = if is_dir { 0 } else { metadata.len() };
        let modified_sec = metadata
            .modified()
            .unwrap_or(SystemTime::UNIX_EPOCH)
            .duration_since(SystemTime::UNIX_EPOCH)
            .map(|d| d.as_secs())
            .unwrap_or(0);

        let mime_type = guess_mime_type(&filename, is_dir);

        let file_entry = FileEntry {
            path: path_str,
            filename,
            extension,
            size_bytes,
            modified_sec,
            is_dir,
            mime_type,
            terms: Vec::new(),
        };

        if db.insert(&file_entry).is_ok() {
            count.fetch_add(1, Ordering::Relaxed);
            total_bytes.fetch_add(size_bytes, Ordering::Relaxed);
        }
    });

    let elapsed = start_time.elapsed().as_millis();
    Ok(IndexerStats {
        files_indexed: count.load(Ordering::Relaxed),
        bytes_indexed: total_bytes.load(Ordering::Relaxed),
        elapsed_ms: elapsed,
    })
}

fn guess_mime_type(filename: &str, is_dir: bool) -> String {
    if is_dir {
        return "inode/directory".to_string();
    }
    if filename.ends_with(".rs") {
        "text/x-rust".to_string()
    } else if filename.ends_with(".cpp") || filename.ends_with(".h") {
        "text/x-c++src".to_string()
    } else if filename.ends_with(".py") {
        "text/x-python".to_string()
    } else if filename.ends_with(".json") {
        "application/json".to_string()
    } else if filename.ends_with(".png") || filename.ends_with(".jpg") {
        "image/png".to_string()
    } else if filename.ends_with(".pdf") {
        "application/pdf".to_string()
    } else {
        "application/octet-stream".to_string()
    }
}
