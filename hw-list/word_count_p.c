/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"
//execute in one thread
void init_words(word_count_list_t* wclist) { /* TODO */
  list_init(&wclist->lst);
  pthread_mutex_init(&wclist->lock, NULL);
}

size_t len_words(word_count_list_t* wclist) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  size_t len = 0;
  const struct list_elem *wc = list_begin(&wclist->lst);
  const struct list_elem *const end = list_tail(&wclist->lst);
  for (; wc != end; wc = wc->next) {
    len++;
  }
  pthread_mutex_unlock(&wclist->lock);
  return len;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  word_count_t *ret = NULL;
  for (int i = 0; word[i]; ++i) {
    word[i] = tolower(word[i]);
  }
  struct list_elem *le = list_begin(&wclist->lst);
  const struct list_elem *en = list_tail(&wclist->lst);
  for (; le != en; le = le->next) {
    const char *cstr = list_entry(le, word_count_t, elem)->word;
    if (strcmp(cstr, word) == 0) {
      ret = list_entry(le, word_count_t, elem);
      break;
    }
  }
  pthread_mutex_unlock(&wclist->lock);
  return ret;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  word_count_t *ret;
  if ((ret = find_word(wclist, word)) != NULL) ret->count++;
  else {
    pthread_mutex_lock(&wclist->lock);
    word_count_t *new_wct = (word_count_t*)malloc(sizeof(word_count_t));
    new_wct->count = 1;
    new_wct->word = word;
    list_push_back(&wclist->lst, &new_wct->elem);
    ret =new_wct;
    pthread_mutex_unlock(&wclist->lock);
  }
  return ret;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) { /* TODO */

  struct list_elem *le = list_begin(&wclist->lst);
  struct list_elem *en = list_tail(&wclist->lst);
  for (; le != en; le = le->next) {
    const word_count_t *wct = list_entry(le, word_count_t, elem);
    const char *cstr = wct->word;
    printf("%s ", cstr);
    printf("%d\n", wct->count);
  }
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  /* TODO */
  const word_count_t *wc1 = list_entry(ewc1, word_count_t, elem);
  const word_count_t *wc2 = list_entry(ewc2, word_count_t, elem);
  return (*(bool (*)(const word_count_t*, const word_count_t*))aux)(wc1, wc2);
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  list_sort(&wclist->lst, less_list, less);
}

