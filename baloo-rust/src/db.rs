use serde::{Deserialize, Serialize};
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
        Ok(())
    }

    pub fn search_filename(&self, query: &str) -> Vec<FileEntry> {
        let query_lower = query.to_lowercase();
        let mut results = Vec::new();

        for item in self.db.iter() {
            if let Ok((_, value)) = item {
                if let Ok(entry) = serde_json::from_slice::<FileEntry>(&value) {
                    if entry.filename.to_lowercase().contains(&query_lower)
                        || entry.path.to_lowercase().contains(&query_lower)
                        || entry.extension.to_lowercase().contains(&query_lower)
                    {
                        results.push(entry);
                        if results.len() >= 500 {
                            break;
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
        // Sled flushes and compacts logs
        self.db.flush()?;
        Ok(())
    }
}
