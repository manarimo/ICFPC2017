pipeline {
    agent any

    stages {
        stage ('Build') {
            steps {
                sh './bin/build.sh ${env.GIT_COMMIT} ${env.GIT_BRANCH} ${env.GIT_PREVIOUS_SUCCESSFUL_COMMIT}'
            }
        }
        stage ('Run') {
            steps {
                sh './bin/run.sh ${env.GIT_COMMIT} ${env.GIT_BRANCH}'
            }
        }
    }
}