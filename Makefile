# import build config
# You can change the default build config with `make conf="build_special.env" release`
conf ?= .circleci/ci.env
include $(conf)
export $(shell sed 's/=.*//' $(conf))

APP_VERSION := $(shell .circleci/fetch_version.sh)

# HELP
# This will output the help for each task
# thanks to https://marmelab.com/blog/2016/02/29/auto-documented-makefile.html
.PHONY: help

help: ## This help.
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

.DEFAULT_GOAL := help


# GIT TASKS
git-tag: ## Tag the latest commit
	git tag -m 'Automated release' v$(APP_VERSION)

git-push: ## Push tag to the upstream
	git push origin --tags

# DOCKER TASKS
build: ## Build the image
	docker build \
		--file $(CLIENT_PATH)/Dockerfile \
		--tag $(APP_NAME) $(CLIENT_PATH)

build-nc: ## Build the image without caching
	docker build --no-cache --force-rm \
		--file $(CLIENT_PATH)/Dockerfile \
		--tag $(APP_NAME) $(CLIENT_PATH)

push: push-latest push-version ## Push the `{version}` and `latest` tagged images to the registry

push-latest: tag-latest ## Push the `latest` taged container to the registry
	@echo 'push latest to the Docker registry'
	docker push $(IMAGE_NAME):latest

push-version: tag-version ## Push the `{version}` taged container to the registry
	@echo 'push $(APP_VERSION) to to the Docker registry'
	docker push $(IMAGE_NAME):$(APP_VERSION)

tag: tag-latest tag-version ## Tag the image with the `{version}` and `latest` tags

tag-latest: ## Tag the image with the `{version}` tag
	@echo 'create tag latest'
	docker tag $(APP_NAME) $(IMAGE_NAME):latest

tag-version: ## Tag the image with the `latest` tag
	@echo 'create tag $(APP_VERSION)'
	docker tag $(APP_NAME) $(IMAGE_NAME):$(APP_VERSION)

client-test: client-run ## Run and test Docker image of the web client
	@docker run --rm --network container:$(APP_NAME)-test \
		curlimages/curl:7.80.0 \
		--retry 5 \
		--retry-connrefused \
		http://localhost | grep "<title>Environment control</title>"
	@printf "Client test \e[32mPASSED\e[0m\n"
	@docker rm -vf $(APP_NAME)-test

client-run: build ## Build and run Docker image of the web client
	@echo 'running container $(APP_NAME)-test from image $(APP_NAME)'
	@echo 'container is up with ID: $(shell docker run -d --name $(APP_NAME)-test -p 8080:80 $(APP_NAME))'

client-rm: ## Stop and remove web client container
	@docker rm -vf $(APP_NAME)-test

client-deploy: ## Deploy web client to the server. Pass APP_VERSION=x.y.z to override version
	@echo 'deploying web client'
	@( APP_VERSION=$(APP_VERSION) docker-compose --project-name $(APP_NAME) \
		--file client/docker-compose.yml \
		--host "ssh://$(DEPLOY_USER)@$(DEPLOY_HOST)" \
		up -d	)

	@printf "\n\e[33mWeb client is available at http://$(DEPLOY_HOST):8081\e[0m\n"

	@printf "\n\e[33mTo interact with containers perform: 'export APP_VERSION=$(APP_VERSION)' \n\
and use docker-compose like that: \n\
'docker-compose -p $(APP_NAME) -f client/docker-compose.yml -H "ssh://$(DEPLOY_USER)@$(DEPLOY_HOST)" <command>' \e[0m\n\n"

# RELEASE TASKS
docker-release: build-nc push ## Build `{version}` and `latest` tagged Docker images and push'em to the registry

release: git-tag git-push ## Tag the latest master with version from include/version.h and push to the upstream
	@printf "\n\e[33mCheck release status at https://app.circleci.com/pipelines/github/vi7/arduino-grower\e[0m\n"


# HELPERS
version: ## Output the current version
	@echo $(APP_VERSION)
