# Dataluge

data:
da·ta
ˈdatə,ˈdātə/noun
1. facts and statistics collected together for reference or analysis.

luge: 
luge
lo͞oZH/noun
1. a light toboggan for one or two people, ridden in a sitting or supine position.

Moving a database from one server to another via backup/restore requires a lot of extra IO and you end up with files laying around.
Dataluge allows you to stream the backup from one server to another using the virtual device interface for SQL Server backups.

---

### High level process:
1. Create virtual interface on target (source or destination)
2. Kick off a thread running the backup/restore
3. Kick off another thread that:
	a) reads from the virtual interface and writes to a socket (backup)
	b) reads from a socket and writes to the virtual interface (restore)
4. fin

---
There is a need for a "client" install on both the source and destination as the virtual interfaces cannot be created or accessed remotely.
TODO: Windows Service? Just an EXE that's kicked off?



