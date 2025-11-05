#!\user\bin\bash

docker build -t landingpage/curso-ia:latest .
docker run -d -p 80:80 landingpage/curso-ia:latest

az login
az acr login --name acrpollidioazure

docker tag landingpage/curso-ia:latest acrpollidioazure.azurecr.io/landingpage/curso-ia:latest
docker push acrpollidioazure.azurecr.io/landingpage/curso-ia:latest

az aks get-credentials --resource-group aks-dev-polli-dio-azure-grp --name aks-dev-polli-dio-azure

kubectl apply -f deployment.yaml
kubectl apply -f service.yaml

kubectl get svc curso-ia-service
