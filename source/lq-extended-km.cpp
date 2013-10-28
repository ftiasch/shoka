int link[N],next[N];
bool hungary(int x)
{
    f[x]=true;
    for (int i=1;i<=n;i++)
    {
        if (g[i])
            continue;
        int d=dx[x]+dy[i]-w[x][i];
        if (!d)
        {
            g[i]=true;
            if (b[i])
            {
                link[x]=i;
                next[x]=0;
                return(true);
            }
            for (int j=1;j<=n;j++)
            {
                if (f[j])
                    continue;
                if (c[j][i] && hungary(j))
                {
                    link[x]=i;
                    next[x]=j;
                    return(true);
                }
            }
        }
        else if (d<slack[i])
            slack[i]=d;
    }
    return(false);
}
void push(int x)
{
    int d=a[x];
    for (int i=x;i;i=next[i])
    {
        if (next[i] && d>c[next[i]][link[i]])
            d=c[next[i]][link[i]];
        if (!next[i] && d>b[link[i]])
            d=b[link[i]];
    }
    a[x]-=d;
    for (int i=x;i;i=next[i])
    {
        if (next[i])
            c[next[i]][link[i]]-=d;
        else
            b[link[i]]-=d;
        c[i][link[i]]+=d;
    }
}

int main()
{
    while (1)
    {
        scanf("%d",&n);
        if (n==0)
            break;
        for (int i=1;i<=n;i++)
        {
            scanf("%d%d%d%d",&X[i],&Y[i],&Z[i],&a[i]);
            b[i]=a[i];
            dx[i]=-inf;
        }
        memset(dy,0,sizeof(dy));
        memset(c,0,sizeof(c));
        for (int i=1;i<=n;i++)
            for (int j=1;j<=n;j++)
            {
                if (i==j)
                    w[i][j]=-inf;
                else
                    w[i][j]=-Sqrt(sqr(X[i]-X[j])+sqr(Y[i]-Y[j])+sqr(Z[i]-Z[j]));
                if (w[i][j]>dx[i])
                    dx[i]=w[i][j];
            }
        for (int i=1;i<=n;i++)
            while (1)
            {
                for (int j=1;j<=n;j++)
                    slack[j]=inf;
                while (a[i])
                {
                    memset(f,0,sizeof(f));
                    memset(g,0,sizeof(g));
                    if (hungary(i))
                        push(i);
                    else
                        break;
                }
                if (!a[i])
                    break;
                int d=inf;
                for (int i=1;i<=n;i++)
                    if (!g[i] && slack[i]<d)
                        d=slack[i];
                for (int i=1;i<=n;i++)
                    if (f[i])
                        dx[i]-=d;
                for (int i=1;i<=n;i++)
                    if (g[i])
                        dy[i]+=d;
            }
        int ans=0;
        bool flag=false;
        for (int i=1;i<=n;i++)
        {
            for (int j=1;j<=n;j++)
                ans+=c[i][j]*w[i][j];
            if (c[i][i]>0)
            {
                flag=true;
                break;
            }
        }
        if (flag)
            ans=1;
        printf("%d\n",-ans);
    }
    return(0);
}

