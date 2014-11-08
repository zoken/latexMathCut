class WordPos {
    public:
        char    *word;
        int     pos;
        bool    bcw;  // 用户自定义词 ? true : false
        int     cls;  // (用户自定义词 && TC_CLS) ? 用户自定义词分类号 : NULL
};
