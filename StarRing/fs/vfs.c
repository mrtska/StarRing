/*
[Module vfs.c]
Copyright(c) 2015 mrtska.starring

This software is released under the MIT License.

http://opensource.org/licenses/mit-license.php
Created on: 2014/09/28
*/


#include <fs/vfs.h>
#include <tree.h>
#include <string.h>
#include <slab.h>
#include <list.h>
#include <system.h>
#include <apic.h>
#include <smp.h>

#include <fs/stat.h>

//ルートファイルシステムマウントポイント
struct tree *fs_tree;

static struct kmem_cache *fs_node_cache;
static struct kmem_cache *file_system_cache;

//FIXME PF 0xFFFF8810006AAAA8
void vfs_init(void) {

	fs_tree = tree_create();

	struct vfs_entry *entry = kmalloc(sizeof(struct vfs_entry), 0);

	entry->name = "[root]";
	entry->file = NULL;


	tree_set_root(fs_tree, entry);

	fs_node_cache = kmem_cache_create("fs node", sizeof(struct fs_node), 0, 0, NULL);
	file_system_cache = kmem_cache_create("file system", sizeof(struct file_system), 0, 0, NULL);
}


struct fs_node *alloc_fs_node(void) {

	return kmem_cache_alloc(fs_node_cache, 0);
}

//ファイルシステムを管理する構造体のメモリを確保する
struct file_system *alloc_file_system(void) {

	return kmem_cache_alloc(file_system_cache, 0);
}




void vfs_mount(char *path, struct fs_node *node) {

	//pathを分割するときに使うバッファ
	char *tokbuf = kmalloc(0x100, 0);
	strcpy(tokbuf, path);
	char *tok = strtok(tokbuf, '/');

	//VFSにマウントするときは絶対パスでなければならない
	if(path[0] != '/') {

		kprintf("[vfs_mount] must be absolute path!\n");
		return;
	}


	if(strlen(path) == 1) {

		((struct vfs_entry*)fs_tree->root->value)->file = node;
		kprintf("root %p\n", 	((struct vfs_entry*)fs_tree->root->value)->file->fs->opt->finddir);
		kfree(tokbuf, 0x100);
		return;
	}

	//アンセスタノード
	struct tree_node *parent = fs_tree->root;
	struct list_head *cur_children = &parent->children;

	//strtokが0を返すまでループ
	while(tok != 0x00) {

		struct list_head *ptr;
		struct tree_node *child;
		list_for_each(ptr, cur_children) {

			child = list_entry(ptr, struct tree_node, children);
			//ファイル/ディレクトリが見つかった場合
			if(strcmp(tok, ((struct vfs_entry*) child->value)->name) == 0) {

				parent = child;
				//kprintf("existed = %s\n", ((struct vfs_entry*) child->value)->name);
				goto next;
			}
		}
		//ファイル/ディレクトリが見つからなかった場合
		struct tree_node *new_child = kmalloc(sizeof(struct tree_node), 0);
		struct vfs_entry *entry = kmalloc(sizeof(struct vfs_entry), 0);
		entry->name = tok;
		new_child->value = entry;
		new_child->parent = parent;
		parent = new_child;

		entry->file = kmalloc(sizeof(struct fs_node), 0);

		//ファイルネームだけ書き込む ディレクトリだけの場合
		strcpy(entry->file->filename, tok);

		entry->file->flags = FS_MOUNTPOINT;

		list_add_tail(&new_child->children, cur_children);

next:
		tok = strtok(0, '/');

	}

	((struct vfs_entry*) parent->value)->file = node;
}


static struct fs_node *get_mount_point(char *path) {

	struct fs_node *mount_point = kmalloc(sizeof(struct fs_node), 0);

	//pathを分割するときに使うバッファ
	char *tokbuf = kmalloc(0x100, 0);
	strcpy(tokbuf, path);
	char *tok = strtok(tokbuf, '/');
	//アンセスタノード & ペアレントノード
	struct tree_node *parent = fs_tree->root;
	struct list_head *cur_children = &parent->children;
	//strtokが0を返すまでループ
	while(tok != 0x00) {

		struct list_head *ptr;
		struct tree_node *child;
		list_for_each(ptr, cur_children) {

			child = list_entry(ptr, struct tree_node, children);
			//ファイル/ディレクトリが見つかった場合
			if(strcmp(tok, ((struct vfs_entry*) child->value)->name) == 0) {

				parent = child;
				mount_point = ((struct vfs_entry*) child->value)->file;
				//kprintf("existed = %s\n", ((struct vfs_entry*) child->value)->name);
				goto next;
			}
		}
next:
		tok = strtok(0, '/');
	}
	if(mount_point->filename[0] == 0x00) {

		//マウントポイントがルートだった場合
		mount_point = ((struct vfs_entry*)fs_tree->root->value)->file;
	}
	return mount_point;
}




//カーネル用ファイルオープン クローズする必要はない
struct fs_node *kopen(const char *name, unsigned int flags) {


	kprintf("[kopen] path %s\n", name);
	//TODO 相対パスを絶対パスに変換する
	char *path = (char*) name;

	//マウントポイントをもとに処理する
	struct fs_node *node = finddir_fs(get_mount_point(path), path);


	//TODO ファイルが見つからなかった場合

	if(node == NULL) {

		kprintf("[kopen] No such file or directory %s\n", path);
		return NULL;
	}
	open_fs(node, 0);

	return node;
}

struct fs_node *get_node(unsigned int fd) {


	struct process *process = get_process();
	struct list_head *p;
	list_for_each(p, &process->fd_list) {

		struct file_descriptor *desc = list_entry(p, struct file_descriptor, list);
		if(fd == desc->fd) {

			return desc->node;
		}
	}

	return NULL;
}
struct file_descriptor *get_file_descriptor(unsigned int fd) {


	struct process *process = get_process();
	struct list_head *p;
	list_for_each(p, &process->fd_list) {

		struct file_descriptor *desc = list_entry(p, struct file_descriptor, list);
		if(fd == desc->fd) {

			return desc;
		}
	}

	return NULL;
}



//ファイルシステムからファイルを読み込む
unsigned int read_fs(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {

	if(node->fs->opt->read) {

		return node->fs->opt->read(node, offset, size, buffer);
	}
	trace();

	return -1;
}

//ファイルシステムにファイルを書き込む
unsigned int write_fs(struct fs_node *node, unsigned int offset, unsigned int size, unsigned char *buffer) {

	if(node->flags & FS_READONLY) {

		return -1;
	}

	if(node->fs->opt->write) {

		return node->fs->opt->write(node, offset, size, buffer);
	}
	trace();
	return -1;
}

//ファイルシステムにファイルを書き込む
unsigned int writev_fs(struct fs_node *node, const struct iovec* vec, unsigned long len) {

	if(node->flags & FS_READONLY) {

		return -1;
	}

	if(node->fs->opt->writev) {

		return node->fs->opt->writev(node, vec, len);
	}

	int ret = 0;
	int i;
	for(i = 0; i < len; i++) {

		const struct iovec *v = &vec[i];

		ret = write_fs(node, 0, v->iov_len, v->iov_base);
		if(ret == -1) {

			goto fail;
		}
	}

	return ret;

fail:
	trace();
	return -1;
}


//ファイルをオープンする
void open_fs(struct fs_node *node, unsigned int flags) {

	if(node->fs->opt->open) {

		node->fs->opt->open(node, flags);


		return;
	}
	trace();
}

//ファイルをクローズする
void close_fs(struct fs_node *node) {

	if(node->fs->opt->close) {

		node->fs->opt->close(node);
		return;
	}
	trace();
}

//ファイルをサーチする
struct fs_node *finddir_fs(struct fs_node *node, char *name) {

	if(node->fs->opt->finddir) {

		return node->fs->opt->finddir(node, name);
	}

	//キャラクタデバイスの場合
	if(node->flags & FS_CHARDEVICE) {

		return node;
	}
	kprintf("[kernel/finddir_fs] invalid node\n");
	return NULL;
}

int fstat_fs(struct fs_node *node, struct stat *stat) {

	if(node->fs->opt->fstat) {

		return node->fs->opt->fstat(node, stat);
	}

	kprintf("could not execute fstat %s\n", node->filename);
	return -1;
}






















