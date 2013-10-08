#include <string.h>
#include <tap.h>
#include "sse.h"

static inline int
isign(int a)
{
    return a < 0 ? -1 : ! !a;
}

int
main(void)
{
    plan_tests(67);

#   define TRY(tgt,pat) ok(ssestr(tgt,pat) == strstr(tgt,pat), "'%s'~'%s' -> %p:%p", \
                            tgt, pat, ssestr(tgt,pat), strstr(tgt,pat))
    TRY("", "a");
    TRY("hello", "");
    TRY("art", "xe");

    TRY("hello", "h");
    TRY("hello", "e");
    TRY("hello", "l");
    TRY("hello", "p");

    TRY("ab", "cd");
    TRY("ab", "ab");
    TRY("cab", "ab");
    TRY("dcab", "ab");
    TRY("edcab", "ab");
    TRY("edcabe", "ab");
    TRY("fedcabf", "ab");

    char    ab[] = "ab";

    TRY(ab, "abc");
    TRY("abc", "xbc");
    TRY("abc", "axc");
    TRY("abc", "abx");
    TRY("abc", "abc");
    TRY("abcd", "abc");
    TRY("zabcd", "abc");
    TRY("yzabcd", "abc");
    TRY("xyzabcd", "abc");
    TRY("wxyzabcd", "abc");
    TRY("xabc", "abc");

    TRY("", "abcd");
    TRY("a", "abcd");
    TRY("ab", "abcd");
    TRY("abcd", "xbcd");
    TRY("abcd", "axcd");
    TRY("abcd", "abxd");
    TRY("abcd", "abcx");
    TRY("abcd", "abcd");
    TRY("abcde", "abcd");
    TRY("zabcd", "abcd");
    TRY("yzabcd", "abcd");
    TRY("xyzabcd", "abcd");
    TRY("vwxyzabcd", "abcd");
    TRY("uvwxyzabcd", "abcd");
    TRY("tuvwxyzabcd", "abcd");
    TRY("stuvwxyzabcd", "abcd");
    TRY("rstuvwxyzabcd", "abcd");
    TRY("qrstuvwxyzabcd", "abcd");
    TRY("qrstuvwxyzabcdabcd", "abcd");
    TRY("xabcde", "abcd");

    TRY("", "abcde");
    TRY("a", "abcde");
    TRY("ab", "abcde");
    TRY("abcde", "xbcd");
    TRY("abcde", "axcd");
    TRY("abcde", "abxd");
    TRY("abcde", "abcx");
    TRY("abcde", "abcde");
    TRY("abcdee", "abcde");
    TRY("zabcde", "abcde");
    TRY("yzabcde", "abcde");
    TRY("xyzabcde", "abcde");
    TRY("vwxyzabcde", "abcde");
    TRY("uvwxyzabcde", "abcde");
    TRY("tuvwxyzabcde", "abcde");
    TRY("stuvwxyzabcde", "abcde");
    TRY("rstuvwxyzabcde", "abcde");
    TRY("qrstuvwxyzabcde", "abcde");
    TRY("qrstuvwxyzabcdeabcde", "abcde");
    TRY("xabcdee", "abcde");
    char    y[] =
        "451AC87F_1271_3613_1\t2006-09-27T18:52:48|f=<pgsql-bugs-owner+M15844@postgresql.org>|t=<mischa@home.com>|h=__FRAUD_419_REFNUM|h=__HAS_MSGID|h=__MIME_TEXT_ONLY|h=__SANE_MSGID|Size=7477|inbound|action=deliver|p=0.076|S=?q?[BUGS]_BUG_#2653:_failed_to_make|b=ok|r=200.46.204.254|tm=1.00|a=a/eom";
    TRY(y, "009");
    TRY(y, "a=a");

    return exit_status();
}
