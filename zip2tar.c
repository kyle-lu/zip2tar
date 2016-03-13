#include <sys/types.h>
#include <sys/stat.h>

#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void	zip2tar(const char *, const char *);
static int	copy_data(struct archive *, struct archive *);
static void	msg(const char *);
static void	errmsg(const char *);
static void	usage(void);

int
main(int argc, const char **argv)
{
	if (argc < 3) usage();

	zip2tar(argv[1], argv[2]);

	return (0);
}


static void
zip2tar(const char *zipfile, const char *tarfile)
{
	struct archive *zip;
	struct archive *tar;
	struct archive_entry *entry;
	int r;

	zip = archive_read_new();
	tar = archive_write_new();
	archive_read_support_format_zip(zip);
	archive_write_set_format_ustar(tar);
	if (zipfile != NULL && strcmp(zipfile, "-") == 0)
		zipfile = NULL;
	if (tarfile != NULL && strcmp(tarfile, "-") == 0)
		tarfile = NULL;
	if ((r = archive_read_open_filename(zip, zipfile, 10240))) {
		errmsg(archive_error_string(zip));
		errmsg("\n");
		exit(r);
	}
	if ((r = archive_write_open_filename(tar, tarfile))) {
		errmsg(archive_error_string(tar));
		errmsg("\n");
		exit(r);
	}
	for (;;) {
		r = archive_read_next_header(zip, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
			errmsg(archive_error_string(zip));
			errmsg("\n");
			exit(1);
		}
		msg("> ");
		msg(archive_entry_pathname(entry));
		msg("\n");
		r = archive_write_header(tar, entry);
		if (r != ARCHIVE_OK)
			errmsg(archive_error_string(tar));
		else
			copy_data(zip, tar);
	}
	archive_read_close(zip);
	archive_read_finish(zip);
	archive_write_close(tar);
	archive_write_finish(tar);
	exit(0);
}

static int
copy_data(struct archive *ar, struct archive *aw)
{
	int r;
	const void *buff;
	size_t size;
	off_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF) {
			return (ARCHIVE_OK);
		}
		if (r != ARCHIVE_OK)
			return (r);
		r = archive_write_data(aw, buff, size);
		if (r != size) {
			errmsg(archive_error_string(aw));
			return (ARCHIVE_FAILED);
		}
	}
}

static void
msg(const char *m)
{
	write(1, m, strlen(m));
}

static void
errmsg(const char *m)
{
	write(2, m, strlen(m));
}

static void
usage(void)
{
	const char *m = "Usage: zip2tar <zip file> <tar file>\n";
	errmsg(m);
	exit(1);
}
