int farmland()
{
    for (int i=1;i<=n;i++)
    {
        vector <pair <double,int> > lq;
        for (int j=b[i];j;j=a[j][1])
        {
            int x=a[j][0];
            lq.push_back(make_pair(atan2(p[x].y-p[i].y,p[x].x-p[i].x),j));
        }
        sort(lq.begin(),lq.end());
        for (int j=0;j<lq.size();j++)
            to[lq[(j+1)%lq.size()].second^1]=lq[j].second;
    }
    memset(vis,0,sizeof(vis));
    int ans=0;
    for (int i=2;i<=tot;i++)
        if (!vis[i])
        {
            ll area=0;
            for (int j=i;!vis[j];j=to[j])
            {
                area+=det(p[a[j^1][0]],p[a[j][0]]);
                vis[j]=true;
            }
            if (area>0)
                ans++;
        }
    return(ans);
}

