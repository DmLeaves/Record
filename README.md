# Record

A Qt-based desktop application for managing account and password records. The project has been fully refactored to provide a modular architecture, modern widgets, and a streamlined user experience.

## Features

- Secure unlock flow with master password creation and verification (SHA3-512 hashing).
- Responsive, searchable record browser with instant detail preview and copy-to-clipboard actions.
- Dedicated dialogs for record editing and batch import from structured text files.
- System tray integration for quick locking/unlocking and background operation.
- SQLite-backed data storage with automatic schema migration.

## Building

```bash
qmake LEARNING/LEARNING.pro
make
```

The application stores its data under the OS specific `AppDataLocation` (falls back to `~/.record/data.db` when unavailable).

## Import file format

Each record is separated by a blank line. Within a record the lines should appear as:

1. Title
2. Username
3. Password
4. Category (optional)

Lines starting with `//` are treated as comments and ignored during import.
