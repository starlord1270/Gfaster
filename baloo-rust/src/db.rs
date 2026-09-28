use serde::{Deserialize, Serialize};
use std::collections::HashSet;
use std::path::Path;

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FileEntry {
    pub path: String,
    pub filename: String,
    pub extension: String,
    pub size_bytes: u64,
    pub modified_sec: u64,
    pub is_dir: bool,
    pub mime_type: String,
    pub terms: Vec<String>,
}

pub struct IndexDatabase {
    db: sled::Db,
}

impl IndexDatabase {
    pub fn open<P: AsRef<Path>>(path: P) -> Result<Self, Box<dyn std::error::Error>> {
        let db = sled::open(path)?;
        Ok(Self { db })
    }

    pub fn insert(&self, entry: &FileEntry) -> Result<(), Box<dyn std::error::Error>> {
        let key = entry.path.as_bytes();
        let encoded = serde_json::to_vec(entry)?;
        self.db.insert(key, encoded)?;

        // Index terms in separate tree for sub-millisecond lookup
        if let Ok(terms_tree) = self.db.open_tree("terms") {
            let words = extract_terms(&entry.filename);
            for word in words {
                let term_key = format!("{}:{}", word.to_lowercase(), entry.path);
                let _ = terms_tree.insert(term_key.as_bytes(), key);
            }
        }
        Ok(())
    }

    pub fn search_filename(&self, query: &str) -> Vec<FileEntry> {
        let query_lower = query.to_lowercase().trim().to_string();
        if query_lower.is_empty() {
            return Vec::new();
        }

        let mut results = Vec::new();
        let mut seen_paths = HashSet::new();

        // 1. Instant term index lookup (< 1ms)
        if let Ok(terms_tree) = self.db.open_tree("terms") {
            let prefix = format!("{}:", query_lower);
            for item in terms_tree.scan_prefix(prefix.as_bytes()) {
                if let Ok((_, path_bytes)) = item {
                    if let Ok(Some(val)) = self.db.get(&path_bytes) {
                        if let Ok(entry) = serde_json::from_slice::<FileEntry>(&val) {
                            if seen_paths.insert(entry.path.clone()) {
                                results.push(entry);
                                if results.len() >= 200 {
                                    return results;
                                }
                            }
                        }
                    }
                }
            }
        }

        // 2. Secondary prefix fallback if term match is empty
        if results.len() < 50 {
            for item in self.db.iter() {
                if let Ok((_, value)) = item {
                    if let Ok(entry) = serde_json::from_slice::<FileEntry>(&value) {
                        if entry.filename.to_lowercase().contains(&query_lower)
                            || entry.path.to_lowercase().contains(&query_lower)
                        {
                            if seen_paths.insert(entry.path.clone()) {
                                results.push(entry);
                                if results.len() >= 200 {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        results
    }

    pub fn count(&self) -> usize {
        self.db.len()
    }

    pub fn size_on_disk(&self) -> u64 {
        self.db.size_on_disk().unwrap_or(0)
    }

    pub fn clear(&self) -> Result<(), Box<dyn std::error::Error>> {
        self.db.clear()?;
        Ok(())
    }

    pub fn compact(&self) -> Result<(), Box<dyn std::error::Error>> {
        self.db.flush()?;
        Ok(())
    }
}

fn extract_terms(filename: &str) -> Vec<String> {
    filename
        .split(|c: char| !c.is_alphanumeric())
        .filter(|s| s.len() >= 2)
        .map(|s| s.to_lowercase())
        .collect()
}

