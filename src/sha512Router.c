#include "ft_ssl.h"
#include "libft.h"
#include <sys/stat.h>
#include <fcntl.h> 

void sha512GetFd(int fd, t_hash64 *hash) {
	size_t len;
	size_t size = 0;
	unsigned char input[128];
	unsigned char buffer[128];

	bzero(input, 128);
	while ((len = read(fd, buffer, 128)) > 0) {
		if ((size % 128) + len >= 64) {
			ft_memcpy(input + (size % 128), buffer, 64 - (size % 64));
			sha512EncodeBloc(hash, (unsigned int *)input);
			bzero(input, 128);
			ft_memcpy(input, buffer + (128 - (size % 64)), len - (64 - (size % 64)));
		} else {
			ft_memcpy(input + (size % 128), buffer, len);
		}
		size += len;
	}
	sha512Padding(input, size, hash);
}

void sha512GetArg(char *message, t_hash64 *hash) {
	size_t	len = ft_strlen(message);
	size_t	index = 0;
	unsigned char	current[128];

	while (len - index >= 128) {
		sha512EncodeBloc(hash, (unsigned int*)message);
		message += 128;
		index += 128;
	}
	bzero(current, 128);
	ft_memcpy(current, message, len - index);
	sha512Padding(current, len - index, hash);
}

int sha512Router(char **argv) {
	char	*message = NULL;
	t_optpars opt;
	t_hash64	hash;

	options(argv, &message, &opt);
	sha512InitHash(&hash);
	if (message) {
		sha512GetArg(message, &hash);
	} else {
		int fd = 0;
		if (opt.arg && opt.arg[0]) {
			if ((fd = open(opt.arg[0], O_RDONLY)) < 0) {
				ft_dprintf(2, "ERROR: Can't open file `%s'\n", opt.arg[0]);
				exit(1);
			}
		}
		ft_printf("%d\n", fd);
		sha512GetFd(fd, &hash);
	}
	if (!ft_tabfind(opt.opt, "-q")) {
		sha512PrintHash(&hash);
	}
	return 0;
}