#include "utils.h"


void resetFPOS(struct FPOS target)
{
    target.x = target.y = -1;
}


void resetPOS(struct POS target)
{
    target.x = target.y = -1;
}


void setFPOS(struct FPOS target, struct FPOS place, struct FPOS disp)
{
    target.x = place.x + disp.x;
    target.y = place.y + disp.y;
}


void setPOS(struct POS target, struct POS place, struct POS disp)
{
    target.x = place.x + disp.x;
    target.y = place.y + disp.y;
}


struct FPOS posToFPOS(struct POS target)
{
    struct FPOS ret;
    ret.x = (prec)target.x;
    ret.y = (prec)target.y;
    return ret;
}


struct POS fposToPOS(struct FPOS target)
{
    struct POS ret;
    ret.x = (int)target.x;
    ret.y = (int)target.y;
    return ret;
}


struct POS pmult(struct POS target, int multiply)
{
    struct POS ret;
    ret.x = target.x * multiply;
    ret.y = target.y * multiply;
    return ret;
}


struct FPOS fpmult(struct FPOS target, prec multiply)
{
    struct FPOS ret;
    ret.x = target.x * multiply;
    ret.y = target.y * multiply;
    return ret;
}


struct POS pdiv(struct POS target, int multiply)
{
    struct POS ret;
    ret.x = target.x / multiply;
    ret.y = target.y / multiply;
    return ret;
}


struct FPOS fpdiv(struct FPOS target, prec multiply)
{
    struct FPOS ret;
    ret.x = target.x / multiply;
    ret.y = target.y / multiply;
    return ret;
}


unsigned int hash_function(char* name)
{
    unsigned int hash_index = 0;
    for (int i = 0; i < strlen(name); i++)
    {
        int cur_char = (int)(*(name + i));
        hash_index = hash_index * hash_key + cur_char;
    }
    return hash_index % default_hash_size;
}


charToken_ptr create_charToken(char* stream)
{
    charToken_ptr new_token = (charToken_ptr)malloc(sizeof(struct CharToken));
    new_token->stream = strdup(stream);
    new_token->next = NULL;
    return new_token;
}


void destroy_token_chain(charToken_ptr rm_token)
{
    while(rm_token)
    {
        charToken_ptr token_next = rm_token->next;
        free(rm_token);
        rm_token = token_next;
    }
}


splitToken_ptr split_line(char* line)
{
    char *temp_line = strdup(line);
    int cnt = 1;
    char *token = strtok(temp_line, " ");
    charToken_ptr head = create_charToken(token);
    while (token)
    {
        token = strtok(NULL, " ");
        if (!token) break;
        charToken_ptr node = create_charToken(token);
        charToken_ptr sweep = head;
        while (sweep->next){
            sweep = sweep->next;
        }
        cnt++;
        sweep->next = node;
    }
    char** split_array = (char**)malloc(sizeof(char*) * cnt);
    charToken_ptr sweep = head;
    for (int i = 0; i < cnt; i++)
    {
        split_array[i] = sweep->stream;
        sweep = sweep->next;
    }

    *(split_array[cnt-1]+(strlen(split_array[cnt - 1]) - 1)) = '\0';
    free(temp_line);
    destroy_token_chain(head);
    splitToken_ptr splitToken = (splitToken_ptr)malloc(sizeof(struct splitToken));
    splitToken->arrayLen = cnt;
    splitToken->split = split_array;
    return splitToken;
}


splitToken_ptr split_dash(char* word)
{
    splitToken_ptr split = (splitToken_ptr)malloc(sizeof(struct splitToken));
    split->arrayLen = 2;
    int wordLen = strlen(word);
    char* checkpoint = word + wordLen;
    for (int i = wordLen; i > 0; i--)
    {
        checkpoint--;
        if (*checkpoint == '/') {
            *checkpoint = '\0';
            break;
        }
    }
    checkpoint++;
    split->split = (char**)malloc(sizeof(char)*2);
    split->split[0] = strdup(word);
    split->split[1] = strdup(checkpoint);
    return split;
}


void destroy_splitToken(splitToken_ptr rm_token)
{
    for (int i = 0; i < rm_token->arrayLen; i++)
    {
        free(rm_token->split[i]);
    }
    free(rm_token->split);
    free(rm_token);
}


double stopwatch(struct timeval *begin, struct timeval *finish) {
    /*
     This function is used to measure the performance time in quickB
     */
	int sec = finish->tv_sec - begin->tv_sec;
	int usec = finish->tv_usec - begin->tv_usec;
	return (sec + usec / 1e6);
}
