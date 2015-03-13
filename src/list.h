#ifndef LIST_H
#define LIST_H

#define SIZE_INC  16

typedef struct basic_item_t
{
  char   *id;
} BASIC_ITEM;


typedef struct list_t
{
  BASIC_ITEM bi;
  int        size;
  int        n;
  int        item_size;
  int        (*item_free) (void *);
  void       **item;
} LIST;

#define OBJ_ID(p) ((BASIC_ITEM*)p)->id

#ifdef __cplusplus
extern "C" {
#endif

  LIST* list_new (char *id, int size, int item_size);
  int   list_free (LIST *l);
  int   list_dump (LIST *l);

  int   list_add_item (LIST *l, void* item);
  void* list_find_item (LIST *l, char *id);
  void* list_get_item (LIST *l, int i);


#ifdef __cplusplus
}
#endif


#endif
