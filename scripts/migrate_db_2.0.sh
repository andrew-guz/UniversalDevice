#!/usr/bin/env bash

set -e

DB_FILE="$1"

# Check argument
if [ -z "$DB_FILE" ]; then
    echo "Usage: $0 database.db"
    exit 1
fi

# Check file exists
if [ ! -f "$DB_FILE" ]; then
    echo "Error: File '$DB_FILE' not found"
    exit 1
fi

# Create backup before migration
echo "Creating backup..."
cp "$DB_FILE" "${DB_FILE}.backup.$(date +%Y%m%d_%H%M%S)"

echo "Running migration..."

sqlite3 "$DB_FILE" <<'SQL'
PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;

-- Rename column in Commands table (commands -> command)
ALTER TABLE Commands RENAME COLUMN commands TO command;

-- Rename column in Scenarios table (scenarios -> scenario)
ALTER TABLE Scenarios RENAME COLUMN scenarios TO scenario;

-- Remove unused columns from Events table
ALTER TABLE Events DROP COLUMN active;
ALTER TABLE Events DROP COLUMN providerId;
ALTER TABLE Events DROP COLUMN providerType;

-- Update Event table Provider type
UPDATE Events
SET event = json_set(
              event,
              '$.provider.type',
              'event'
           )
WHERE json_valid(event)
  AND json_extract(event, '$.provider.id') LIKE '00000000%';

COMMIT;
PRAGMA foreign_keys=ON;
SQL

echo "Migration completed successfully."
