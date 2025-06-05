## Examples how to start PostgreSQL
docker run --name pgsql -e POSTGRES_DB=universal_device -e POSTGRES_USER=XXX -e POSTGRES_PASSWORD=XXX -v /custom/mount:/var/lib/postgresql/data -p 5432:5432 -d postgres

psql -h 127.0.0.1 -p 5432 -U test -d universal_device
